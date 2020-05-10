#  set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf64")
#	set(OBJCOPY_TARGET "elf64-x86-64")
#	set(CAPABS "${CAPABS} -m64")
#	enable_language(ASM_NASM)
#
#
#	add_custom_command(
#	OUTPUT bootloader
#	COMMAND ${CMAKE_ASM_NASM_COMPILER} -f bin -I${CMAKE_CURRENT_SOURCE_DIR}/../ -o bootloader ${CMAKE_CURRENT_SOURCE_DIR}/bootloader.asm
#	DEPENDS bootloader.asm disk_read_lba.asm
#)
#
#add_custom_target(run ALL DEPENDS bootloader)
#
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bootloader DESTINATION boot)
