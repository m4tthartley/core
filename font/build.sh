
gcc font_generator.c -o ./font_generator.exe -g -Wno-incompatible-pointer-types
if [ $? -eq 0 ]; then
	./font_generator.exe;
fi
