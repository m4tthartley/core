/* date = October 24th 2022 9:16 pm */

#ifdef _UI_HEADER
#undef _UI_HEADER

#include "./math.c"

typedef unsigned char byte;
typedef unsigned __int32 u32;
typedef u32 b32;

typedef struct {
	void* texture;
	char kerning[128];
} ui_font;

// LINKED LIST
//#define llInsert(head, item)
typedef struct ll_node {
	struct ll_node* next;
	struct ll_node* prev;
} ll_node;
void addToList(ll_node** head, ll_node* item) {
	item->next = *head;
	if(*head) {
		(*head)->prev = item;
	}
	*head = item;
}
void addToListBack(ll_node** head, ll_node* item) {
	item->next = 0;
	if(*head) {
		ll_node* w = *head;
		while(w->next) {
			w = w->next;
		}
		w->next = item;
		item->prev = w;
	} else {
		*head = item;
	}
}
void removeFromList(ll_node **head, ll_node* item) {
	if(item->next) {
		item->next->prev = item->prev;
	}
	if(item->prev) {
		item->prev->next = item->next;
	} else {
		*head = item->next;
	}
}
// LINKED LIST

typedef enum {
	UI_FLAG_BOX = 1,
	UI_FLAG_BORDER = 1<<1,
	UI_FLAG_CLICKABLE = 1<<2,
	UI_FLAG_TEXT = 1<<3,
	UI_FLAG_SPRITE = 1<<4,
	UI_FLAG_BEVEL = 1<<5,
	UI_FLAG_WRITABLE = 1<<6,
	UI_FLAG_PARENT = 1<<7,
	UI_FLAG_BACKGROUND = 1<<8,
	UI_FLAG_PERSISTENT_ACTIVE = 1<<9,
} ui_flag;
typedef enum {
	UI_SIZING_CHILDREN = 0,
	UI_SIZING_STATIC = 1,
	//UI_SIZING_PARENT = 2,
	UI_SIZING_FRACTION = 2,
} ui_sizing_type;
typedef struct {
	ui_sizing_type type;
	float value;
} ui_sizing;
typedef enum {
	UI_LAYOUT_V = 0,
	UI_LAYOUT_H = 1,
} ui_layout_type;
typedef struct {
	u32 key;
	char text[64];
	int hot;
	int active;
	int clicked;
	int lastFrameUsed;
} ui_widget_state;
typedef struct ui_widget {
	ll_node node;
	struct ui_widget* children;
	struct ui_widget* parent;
	
	ui_flag flags;
	ui_widget_state* state;
	char* name;
	ui_layout_type layout;
	ui_sizing xSize;
	ui_sizing ySize;
	char* defaultText;
	int charLimit;
	r_sprite sprite;
	
	vec2 pos;
	vec2 size;
	vec2 padding;
	float spacing;
	vec2 cursor;
} ui_widget;
typedef struct {
	ll_node node;
	float age;
	int stride;
} ui_message;
typedef struct {
	vec2* positions;
	int length;
	char* text;
	vec2 size;
} ui_text_layout;
typedef struct {
	memory_arena widgetTree;
	memory_arena widgetState;
	memory_arena msgMemory;
	memory_arena transient;
	ui_widget* widgets;
	ui_message* messages;
	
	vec2 scale;
	vec2 screenSize;
	vec2 mousePos;
	int lbuttonIsDown;
	int leftArrow;
	int rightArrow;
	int upArrow;
	int downArrow;
	char* input;
	
	//ui_widget* hot;
	//ui_widget* active;
	vec2 cursor;
	float blinker;
	int writableCursor;
	ui_font* font;
	int frameCounter;
} ui_state;

void uiFontSize(float size);
void uiWrapWidth(float width);
ui_text_layout uiLayoutText(char* text, ...);
void uiDrawTextLayout(ui_text_layout layout, vec2 pos, ...);
void uiText(char* text, vec2 pos);
void uiMessage(char* str, ...);

#endif
#ifdef _UI_IMPLEMENTATION
#undef _UI_IMPLEMENTATION

#define WIN32_LEAN_AND_MEAN
#include <WINDOWS.H>
#include <gl/GL.H>
#include <STDIO.H>

ui_state* ui;
ui_widget* _widgetParent = 0;
vec2 _paddingDefault = {0.5f, 0.5f};
float _spacingDefault = 0.5f;
float _fontSizeDefault = 1.0f;
float _wrapWidthDefault = 32.0f;
vec2 _padding = {0.5f, 0.5f};
float _spacing = 0.5f;
float _fontSize = 1.0f;
float _wrapWidth = 32.0f;

vec2 _getRenderedTextSize(ui_font* font, char* text, float size, float wrapWidth);

void uiPadding(vec2 value) {
	_padding = value;
}
void uiPaddingf(float value) {
	_padding = _vec2(value,value);
}
void uiSpacing(float value) {
	_spacing = value;
}
void uiFontSize(float size) {
	_fontSize = size;
}
void uiWrapWidth(float width) {
	_wrapWidth = width;
}

ui_sizing uiStaticSize(float value) {
	ui_sizing size;
	size.type = UI_SIZING_STATIC;
	size.value = value;
	return size;
}

ui_sizing uiFractionOfParentSize(float value) {
	ui_sizing size;
	size.type = UI_SIZING_FRACTION;
	size.value = value;
	return size;
}

#define uiGetLayoutAxis(vec, layout) (vec.f[1 - layout])

void uiMessage(char* str, ...) {
	va_list args;
	va_start(args, str);
	int len = vsnprintf(0, 0, str, args) + 1;
	char* buffer = pushMemory(&ui->transient, len);
	vsnprintf(buffer, len, str, args);
	
	ui_message msg = {0};
	msg.stride = sizeof(ui_message) + ntStringLength(buffer);
	if(ui->messages) {
		byte* newAddress = (byte*)ui->msgMemory.address+ui->msgMemory.stack;
		if(newAddress < (byte*)ui->messages+ui->messages->stride &&
		   newAddress+msg.stride > ui->messages) {
			removeFromList(&ui->messages, ui->messages);
		}
	}
	ui_message* m = pushRollingMemory(&ui->msgMemory, msg.stride);
	addToListBack(&ui->messages, m);
	*m = msg;
	++m;
	copyMemory(m, buffer, ntStringLength(buffer));
	
	popMemory(&ui->transient, len);
}

u32 shash(char* str) {
	u32 result = 0x811c9dc5;
	while(*(str++)) {
		result ^= *str;
		result *= 0x01000193;
	}
	return result;
}

b32 uiAnyActiveWidget() {
	ui_widget_state* states = ui->widgetState.address;
	int num = ui->widgetState.stack/sizeof(ui_widget_state);
	for(int i=0; i<num; ++i) {
		if(states[i].active) {
			return 1;
		}
	}
	return 0;
}

ui_widget_state* findWidgetState(ui_widget* widget) {
	u32 hash = shash(widget->name);
	ui_widget_state* states = ui->widgetState.address;
	int maxStates = ui->widgetState.size/sizeof(ui_widget_state);
	for(int i=0; i<maxStates; ++i) {
		if(states[i].key == hash) {
			states[i].lastFrameUsed = ui->frameCounter;
			return states + i;
		}
	}
	
	return 0;
}

ui_widget* uiPushWidget(ui_widget widget) {
	ui_widget* result = pushStruct(&ui->widgetTree, widget);
	result->padding = _padding;
	result->spacing = _spacing;
	result->parent = _widgetParent;
	if(!result->charLimit) result->charLimit = 64;
	if(result->flags&UI_FLAG_CLICKABLE) {
		ui_widget_state* state = findWidgetState(result);
		if(!state) {
			state = pushMemory(&ui->widgetState, sizeof(ui_widget_state));
			state->key = shash(result->name);
			state->lastFrameUsed = ui->frameCounter;
			if(result->flags&UI_FLAG_WRITABLE) {
				copyMemory(state->text, result->defaultText, ntStringLength(result->defaultText));
			}
		}
		result->state = state;
	}
	if(_widgetParent) {
		addToListBack(&_widgetParent->children, result);
	} else {
		addToListBack(&ui->widgets, result);
	}
	return result;
}

void uiPushParent(ui_widget* parent) {
	_widgetParent = parent;
	parent->flags |= UI_FLAG_PARENT;
}
void uiPopParent() {
	_widgetParent = _widgetParent->parent;
}

#define uiWindowParent(name, flags) for(int _i=(uiPushParent(uiPushWindow(name, flags)),0); _i<1; ++_i,uiPopParent())

ui_widget* uiPushText(char *name) {
	ui_widget b = {0};
	b.flags = UI_FLAG_TEXT;
	b.name = name;
	return uiPushWidget(b);
}

ui_widget* uiPushSprite(r_sprite s) {
	ui_widget b = {0};
	b.flags = UI_FLAG_SPRITE;
	b.name = "sprite";
	b.sprite = s;
	ui_widget* w = uiPushWidget(b);
	w->padding = _vec2(0,0);
	return w;
}

ui_widget* uiPushButton(char *name) {
	ui_widget b = {0};
	b.flags = UI_FLAG_BOX | UI_FLAG_BEVEL | UI_FLAG_CLICKABLE | UI_FLAG_TEXT;
	b.name = name;
	return uiPushWidget(b);
}

ui_widget* uiPushInput(char *name, char* defaultText, int charLimit) {
	ui_widget input = {0};
	input.flags = UI_FLAG_BORDER | UI_FLAG_CLICKABLE | UI_FLAG_TEXT | UI_FLAG_WRITABLE | UI_FLAG_PERSISTENT_ACTIVE;
	input.name = name;
	input.defaultText = defaultText;
	input.charLimit = charLimit;
	//input.xSize = uiFractionOfParentSize(1.0f);
	return uiPushWidget(input);
}

ui_widget* uiPushWindow(char* name, ui_flag extraFlags) {
	ui_widget box = {0};
	box.flags = UI_FLAG_BOX | UI_FLAG_BACKGROUND | extraFlags;
	box.name = name;
	return uiPushWidget(box);
}

void uiPushDropdown(char* name, int* result, char** options, int count) {
	uiPaddingf(0);
	ui_widget* window = uiPushWindow(name, UI_FLAG_CLICKABLE | UI_FLAG_PERSISTENT_ACTIVE);
	uiPadding(_paddingDefault);
	//window->xSize = uiFractionOfParentSize(1);
	uiPushParent(window);
	{
		uiSpacing(0);
		ui_widget b = {0};
		b.flags = UI_FLAG_BOX | UI_FLAG_BORDER | UI_FLAG_TEXT;
		char* n = stringFormat("%s:%s", name, options[*result]);
		b.name = n;
		b.xSize = uiFractionOfParentSize(1);
		uiPushWidget(b);
		if(window->state->active) {
			for(int i=0; i<count; ++i) {
				ui_widget b = {0};
				b.flags = UI_FLAG_BOX | UI_FLAG_CLICKABLE | UI_FLAG_TEXT;
				b.name = options[i];
				b.xSize = uiFractionOfParentSize(1);
				if(uiPushWidget(b)->state->clicked) {
					window->state->active = 0;
					*result = i;
				}
			}
		}
		uiSpacing(0.5f);
	}
	uiPopParent();
}

ui_widget* _stepNextWidget(ui_widget* w) { // without children
	//if(w->children) {
		//return w->children;
	//} else 
	if(w->node.next) {
		return w->node.next;
	} 
	//else if(w->parent) {
			//return w->parent->node.next;
//}
else {
		return 0;
	}
}

ui_widget* _stepEveryWidget(ui_widget* w) {
	if(w->children) {
		return w->children;
	} else if(w->node.next) {
		return w->node.next;
	} else if(w->parent) {
		ui_widget* find = w->parent;
		while(!find->node.next) {
			if(find->parent) {
				find = find->parent;
			} else {
				return 0;
			}
		}
		return find->node.next;
	} else {
		return 0;
	}
}

ui_widget* layoutWidgetFirstPass(ui_font* font, ui_widget* w) {
	if(w->flags & UI_FLAG_TEXT) {
		if(w->flags & UI_FLAG_WRITABLE) {
			w->size = uiLayoutText(w->state->text).size;
		} else {
			w->size = uiLayoutText(w->name).size;
		}
	}
	if(w->flags & UI_FLAG_SPRITE) {
		w->size = mul2f(w->sprite.pixelSize, 0.25f);
	}
	if(w->xSize.type == UI_SIZING_STATIC) {
		w->size.x = w->xSize.value;
	}
	if(w->ySize.type == UI_SIZING_STATIC) {
		w->size.y = w->ySize.value;
	}
	
	w->size = add2(w->size, mul2f(w->padding, 2));
	if(/*w->children ||*/ !w->node.next) {
		w->spacing = 0;
	}
	//w->spacing = (!w->children&&w->node.next ? w->spacing : 0);
	
	if(w->children) {
		ui_widget* child = w->children;
		while(child) {
			child = layoutWidgetFirstPass(font, child);
		}
	}
	
	if(w->parent) {
		if(w->parent->layout == UI_LAYOUT_H) {
			w->parent->size.x += w->size.x + w->spacing;
			w->parent->size.y = max(w->parent->size.y, w->size.y + w->parent->padding.y*2);
		} else {
			w->parent->size.y += w->size.y + w->spacing;
			w->parent->size.x = max(w->parent->size.x, w->size.x + w->parent->padding.x*2);
		}
	}
	
	return _stepNextWidget(w);
}

ui_widget* layoutWidgetSecondPass(ui_font* font, ui_widget* w) {
	if(w->xSize.type==UI_SIZING_FRACTION && w->parent) {
		w->size.x = w->parent->size.x*w->xSize.value - w->parent->padding.x*2;
	}
	if(w->ySize.type==UI_SIZING_FRACTION && w->parent) {
		w->size.y = w->parent->size.y*w->ySize.value - w->parent->padding.y*2;
	}
	
	if(w->parent) {
		w->pos = add2(w->parent->pos, w->parent->cursor);
	} else {
		w->pos = ui->cursor;
	}
	
	w->cursor = add2(w->cursor, w->padding);
	
	if(w->children) {
		ui_widget* child = w->children;
		while(child) {
			child = layoutWidgetSecondPass(font, child);
		}
	}
	
	if(w->parent) {
		if(w->parent->layout == UI_LAYOUT_H) {
			w->parent->cursor.x += w->size.x + w->spacing;
		} else {
			w->parent->cursor.y += w->size.y + w->spacing;
		}
	} else {
		ui->cursor.y += w->size.y + w->spacing;
	}
	/*if(w->parent) {
		if(w->parent->layout == UI_LAYOUT_H) {
			w->parent->cursor.x += w->spacing;
		} else {
			w->parent->cursor.y += w->spacing;
		}
	} else {
		ui->cursor.y += w->spacing;
	}*/
	
	return _stepNextWidget(w);
}

void uiLayout(ui_font* font) {
	ui->cursor = _vec2(1,1);
	
	int i = 0;
	ui_widget_state* states = ui->widgetState.address;
	while(i < ui->widgetState.stack/sizeof(ui_widget_state)) {
		if(states[i].lastFrameUsed != ui->frameCounter) {
			copyMemory(states+i, states+i+1, ui->widgetState.stack/sizeof(ui_widget_state)-i);
			popMemory(&ui->widgetState, sizeof(ui_widget_state));
		} else {
			++i;
		}
	}
	
	// first pass
	{
		ui_widget* w = ui->widgets;
		while(w) {
			w = layoutWidgetFirstPass(font, w);
		}
	}
	// second pass
	{
		ui_widget* w = ui->widgets;
		while(w) {
			w = layoutWidgetSecondPass(font, w);
		}
	}
	// clicks
	{
		ui_widget* w = ui->widgets;
		while(w) {
			if(w->flags & UI_FLAG_CLICKABLE) {
				w->state->clicked = 0;
				w->state->hot = 0;
				
				if(ui->mousePos.x > w->pos.x &&
				   ui->mousePos.x < w->pos.x+w->size.x &&
				   ui->mousePos.y > w->pos.y &&
				   ui->mousePos.y < w->pos.y+w->size.y) {
					w->state->hot = 1;
					
					if(ui->lbuttonIsDown) {
						w->state->active = 1;
						if(w->flags & UI_FLAG_WRITABLE) {
							ui->writableCursor = stringLength(w->state->text);
						}
					}
				} else {
					if(!(w->flags&UI_FLAG_PERSISTENT_ACTIVE) && w->state->active) {
						w->state->active = 0;
					}
					if(w->flags&UI_FLAG_PERSISTENT_ACTIVE &&
					   w->state->active &&
					   ui->lbuttonIsDown) {
						w->state->active = 0;
					}
				}
				
				if(w->state->active && !ui->lbuttonIsDown) {
					// click
					if(!(w->flags&UI_FLAG_PERSISTENT_ACTIVE)) {
						w->state->clicked = 1;
						w->state->active = 0;
					}
				}
				
				if(w->state->active && w->flags&UI_FLAG_WRITABLE) {
					int len = stringLength(w->state->text);
					if(ui->input[0] > 31) {
						if(len<w->charLimit-1) {
							byte* text = pushAndCopyMemory(&ui->transient, w->state->text, len);
							copyMemory((byte*)w->state->text+ui->writableCursor+1,
									   text+ui->writableCursor,
									   len-ui->writableCursor);
							w->state->text[ui->writableCursor] = ui->input[0];
							++ui->writableCursor;
							ui->blinker = 0;
							popMemory(&ui->transient, len);
						}
					}
					if(ui->input[0] == 8 && ui->writableCursor>0) {// backspace
						byte* text = pushAndCopyMemory(&ui->transient, w->state->text, len);
						copyMemory((byte*)w->state->text+ui->writableCursor-1,
								   text+ui->writableCursor,
								   len-ui->writableCursor+1);
						//ui->active->state->text[ui->writableCursor] = 0;
						--ui->writableCursor;
						ui->blinker = 0;
						popMemory(&ui->transient, len);
					}
					if(ui->leftArrow) {
						--ui->writableCursor;
						ui->blinker = 0;
					}
					if(ui->rightArrow) {
						++ui->writableCursor;
						ui->blinker = 0;
					}
				}
			}
			
			w = _stepEveryWidget(w);
		}
	}
	
	++ui->frameCounter;
}

void uiRender(game_state* game, float dt) {
	ui_widget* w = ui->widgets;
	
	gfx_sh(game->uiBoxShader);
	glDisable(GL_TEXTURE_2D);
	glColor3f(BLUE.r, BLUE.g, BLUE.b);
	while(w) {
		//if(w->flags & UI_FLAG_BOX) {
		glPushMatrix();
		vec2 pos = w->pos;//sub2(_vec2(1, w->size.y), w->pos);
		gfx_uf2("quad_size", w->size.x, w->size.y);
		gfx_uf2("quad_center", pos.x+w->size.x/2, pos.y+w->size.y/2);
		gfx_uf2("world_position", pos.x, pos.y);
		if(w->state) {
			gfx_ui1("hot", w->flags&UI_FLAG_CLICKABLE && w->state->hot ? 1 : 0);
			gfx_ui1("widgetActive", w->state->active ? 1 : 0);
		}
		gfx_ui1("bevel", w->flags&UI_FLAG_BEVEL ? 1 : 0);
		gfx_ui1("border", w->flags&UI_FLAG_BORDER ? 1 : 0);
		gfx_ui1("uBackground", w->flags&UI_FLAG_BOX ? 1 : 0);
		if(w->flags & UI_FLAG_BACKGROUND) {
			gfx_uf4("uBoxColor", 0.1f, 0.2f, 0.5f, 0.5f);
		} else {
			gfx_uf4("uBoxColor", 0.1f, 0.2f, 0.5f, 1.0f);
		}
		//glTranslatef(pos.x, pos.y, 0);
		glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f(w->size.x, 0);
		glVertex2f(w->size.x, w->size.y);
		glVertex2f(0, w->size.y);
		glEnd();
		glPopMatrix();
		//}
		
		w = _stepEveryWidget(w);
	}
	
#if 0
	gfx_sh(0);
	w = ui->widgets;
	while(w) {
		if(w->state && w->state->active) {
			glPushMatrix();
			glTranslatef(w->pos.x, w->pos.y, 0);
			glColor3f(1,0,0);
			glBegin(GL_LINE_STRIP);
			glVertex2f(0, 0);
			glVertex2f(w->size.x, 0);
			glVertex2f(w->size.x, w->size.y);
			glVertex2f(0, w->size.y);
			glEnd();
			glPopMatrix();
		}
		w = _stepEveryWidget(w);
	}
#endif
	
	// blinker
	gfx_sh(0);
	w = ui->widgets;
	while(w) {
		if(w->state && w->state->active) {
			glColor3f(0,1,0);
			if(w->flags & UI_FLAG_WRITABLE) {
				char tmp = w->state->text[ui->writableCursor];
				w->state->text[ui->writableCursor] = 0;
				ui_text_layout layout = uiLayoutText(w->state->text);
				w->state->text[ui->writableCursor] = tmp;
				
				if(ui->blinker<0.5f) {
					glPushMatrix();
					glTranslatef(w->pos.x+layout.size.x+w->padding.x-0.1f, w->pos.y+w->padding.y*0.5f, 0);
					glBegin(GL_QUADS);
					glVertex2f(0, 0);
					glVertex2f(0.2f, 0);
					glVertex2f(0.2f, w->size.y - w->padding.y);
					glVertex2f(0, w->size.y - w->padding.y);
					glEnd();
					glPopMatrix();
				}
			}
		}
		w = _stepEveryWidget(w);
	}
	
	/*glColor3f(0.15f, 0.3f, 0.75f);
	while(w) {
		if(w->flags & UI_FLAG_BORDER) {
			glPushMatrix();
			vec2 pos = sub2(_vec2(1, ui->screenSize.y-w->size.y), w->pos);
			glTranslatef(pos.x, pos.y, 0);
			glBegin(GL_LINE_STRIP);
			glVertex2f(0, 0);
			glVertex2f(w->size.x, 0);
			glVertex2f(w->size.x, w->size.y);
			glVertex2f(0, w->size.y);
			glEnd();
			glPopMatrix();
		}
		w = _stepNextWidget(w);
	}
	
	w = ui->widgets;*/
	
	// sprites
	gfx_sh(0);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	uiFontSize(1);
	w = ui->widgets;
	while(w) {
		if(w->flags & UI_FLAG_SPRITE) {
			vec2 size = mul2f(w->sprite.pixelSize, 0.25f);
			glBindTexture(GL_TEXTURE_2D, w->sprite.texture.handle);
			glBegin(GL_QUADS);
			glTexCoord2f(w->sprite.uv.x,  w->sprite.uv.y);       glVertex2f(w->pos.x,   w->pos.y+size.y);
			glTexCoord2f(w->sprite.uv2.x, w->sprite.uv.y);       glVertex2f(w->pos.x+size.x, w->pos.y+size.y);
			glTexCoord2f(w->sprite.uv2.x, w->sprite.uv2.y); glVertex2f(w->pos.x+size.x, w->pos.y);
			glTexCoord2f(w->sprite.uv.x,  w->sprite.uv2.y); glVertex2f(w->pos.x,   w->pos.y);
			glEnd();
		}
		w = _stepEveryWidget(w);
	}
	
	ui->blinker += dt;
	ui->blinker = fmod(ui->blinker, 1.0f);
	
	// text
	glBindTexture(GL_TEXTURE_2D, game->fontTexture.handle);
	gfx_sh(0);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	uiFontSize(1);
	w = ui->widgets;
	while(w) {
		if(w->flags & UI_FLAG_WRITABLE) {
			uiText(w->state->text, _vec2(w->pos.x+w->padding.x, w->pos.y+w->padding.y));
		} else if(w->flags & UI_FLAG_TEXT) {
			uiText(w->name, _vec2(w->pos.x+w->padding.x, w->pos.y+w->padding.y));
		}
		w = _stepEveryWidget(w);
	}
	
	// Messages
	{
		glColor3f(0.2f,0.2f,0.2f);
		ui_message* msg = ui->messages;
		int line = 0;
		float maxWidth = 0;
		while(msg) {
			++line;
			ui_text_layout layout = uiLayoutText(msg+1);
			if(layout.size.x > maxWidth) maxWidth = layout.size.x;
			msg = msg->node.next;
		}
		if(line) {
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			glVertex2f(0,    0+line+2);
			glVertex2f(0+maxWidth+2, 0+line+2);
			glVertex2f(0+maxWidth+2, 0);
			glVertex2f(0,    0);
			glEnd();
			glEnable(GL_TEXTURE_2D);
		}
	}
	
	glColor3f(1, 1, 1);
	ui_message* msg = ui->messages;
	int line = 0;
	while(msg) {
		msg->age += dt;
		if(msg->age < 10.0f) {
			uiText(msg+1, _vec2(1, 1+line));
			++line;
		} else {
			removeFromList(&ui->messages, msg);
		}
		msg = msg->node.next;
	}
}

/*void _drawText(ui_font* font, char* text, vec2 pos, float size, float wrapWidth) {
	float x = pos.x;
	float y = pos.y;
	float xx = x;
	float yy = y;
	
	float charSize = 1.0f * size;
	int index=0;
	while(*text) {
		byte kernh = font->kerning[*text] & 0x0f;
		byte kernv = (font->kerning[*text] & 0xf0)>>4;
		float kv = 0.125f*(float)kernv*size;
		
		if(*text == '\n') {
			xx = x;
			yy += charSize*1.5f;
			++text;
			continue;
		}
		
		char* word = text;
		float wordSize = 0;
		while(*word && *word!=' ') {
			wordSize += charSize - (0.125f*(float)kernh*size);
			++word;
		}
		if(xx-x+wordSize > wrapWidth && wordSize<wrapWidth) {
			xx = x;
			yy += charSize*1.5f;
		}
		
		vec2 uv = _vec2((float)(*text%16) / 16.0f, (float)(*text/16) / 8.0f);
		vec2 uvt = _vec2(1.0f/16.0f, 1.0f/8.0f);
		glBegin(GL_QUADS);
		glTexCoord2f(uv.x,       uv.y);       glVertex2f(xx,            yy+kv+charSize);
		glTexCoord2f(uv.x+uvt.x, uv.y);       glVertex2f(xx+(charSize), yy+kv+charSize);
		glTexCoord2f(uv.x+uvt.x, uv.y+uvt.y); glVertex2f(xx+(charSize), yy+kv);
		glTexCoord2f(uv.x,       uv.y+uvt.y); glVertex2f(xx,            yy+kv);
		glEnd();
		
		xx += charSize - (0.125f*(float)kernh*size);
		++text;
		++index;
		if(xx-x > wrapWidth) {
			xx = x;
			yy += charSize*1.5f;
		}
	}
}
vec2 _getRenderedTextSize(ui_font* font, char* text, float size, float wrapWidth) {
	float x = 0.0f;
	float y = 0.0f;
	float xx = x;
	float yy = y;
	
	float charSize = 1.0f * size;
	int index=0;
	while(*text) {
		byte kernh = font->kerning[*text] & 0x0f;
		byte kernv = (font->kerning[*text] & 0xf0)>>4;
		float kv = -0.125f*(float)kernv*size;
		
		if(*text == '\n') {
			xx = x;
			yy += charSize*1.5f;
			++text;
			continue;
		}
		
		char* word = text;
		float wordSize = 0;
		while(*word && *word!=' ') {
			wordSize += charSize - (0.125f*(float)kernh*size);
			++word;
		}
		if(xx-x+wordSize > wrapWidth && wordSize<wrapWidth) {
			xx = x;
			yy += charSize*1.5f;
		}
		
		xx += charSize - (0.125f*(float)kernh*size);
		++text;
		++index;
		if(xx-x > wrapWidth) {
			xx = x;
			yy += charSize*1.5f;
		}
	}
	
	vec2 result;
	result.x = xx;
	result.y = yy + charSize*1.0f;
	return result;
}*/

/*void uiText(ui_font* font, vec2 pos, char* text, ...) {
	va_list args;
	va_start(args, text);
	//float x = va_arg(args, float);
	//float y = va_arg(args, float);
	char buffer[512];
	vsnprintf(buffer, 512, text, args);
	_drawText(font, buffer, pos, _fontSize, _wrapWidth);
	va_end(args);
}*/

ui_text_layout uiLayoutText(char* text, ...) {
	va_list args;
	va_start(args, text);
	int length = vsnprintf(0, 0, text, args);
	vec2* positions = pushMemory(&ui->transient, length*sizeof(vec2));
	ui_text_layout layout;
	layout.positions = positions;
	layout.size = _vec2(0,0);
	layout.length = length;
	layout.text = text;
	
	vec2 pos = _vec2(0,0);
	vec2 cursor = pos;
	
	float charSize = 1.0f*_fontSize;
	int index = 0;
	while(*text) {
		byte kernh = ui->font->kerning[*text] & 0x0f;
		byte kernv = (ui->font->kerning[*text] & 0xf0)>>4;
		float kh = 0.125f*(float)kernh*_fontSize;
		float kv = 0.125f*(float)kernv*_fontSize;
		
		if(*text == '\n') {
			cursor.x = pos.x;
			cursor.y += charSize*1.5f;
			++text;
			++index;
			continue;
		}
		
		char* word = text;
		float wordSize = 0;
		while(*word && *word!=' ') {
			wordSize += charSize - kh;
			++word;
		}
		if(cursor.x-pos.x+wordSize > _wrapWidth && wordSize < _wrapWidth) {
			cursor.x = pos.x;
			cursor.y += charSize*1.5f;
		}
		
		positions[index] = cursor;
		positions[index].y += kv;
		cursor.x += charSize-kh;
		if(cursor.x > layout.size.x) {
			layout.size.x = cursor.x;
		}
		
		if(cursor.x-pos.x > _wrapWidth) {
			cursor.x = pos.x;
			cursor.y += charSize*1.5f;
		}
		
		++text;
		++index;
	}
	
	layout.size.y = cursor.y+charSize;
	return layout;
}

void uiDrawTextLayout(ui_text_layout layout, vec2 pos, ...) {
	va_list args;
	va_start(args, layout.text);
	int length = vsnprintf(0, 0, layout.text, args) + 1;
	char* buffer = pushMemory(&ui->transient, length);
	vsnprintf(buffer, length, layout.text, args);
	
	float charSize = 1.0f*_fontSize;
	for(int i=0; *buffer; ++i,++buffer) {
		if(*buffer != '\n') {
			vec2 uv = _vec2((float)(*buffer%16) / 16.0f, (float)(*buffer/16) / 8.0f);
			vec2 uvt = _vec2(1.0f/16.0f, 1.0f/8.0f);
			vec2 charPos = add2(pos, layout.positions[i]);
			glBegin(GL_QUADS);
			glTexCoord2f(uv.x,       uv.y);       glVertex2f(charPos.x,            charPos.y+charSize);
			glTexCoord2f(uv.x+uvt.x, uv.y);       glVertex2f(charPos.x+(charSize), charPos.y+charSize);
			glTexCoord2f(uv.x+uvt.x, uv.y+uvt.y); glVertex2f(charPos.x+(charSize), charPos.y);
			glTexCoord2f(uv.x,       uv.y+uvt.y); glVertex2f(charPos.x,            charPos.y);
			glEnd();
		}
	}
}

void uiText(char* text, vec2 pos) {
	ui_text_layout layout = uiLayoutText(text);
	uiDrawTextLayout(layout, pos);
}

#endif