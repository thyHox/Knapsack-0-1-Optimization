CC = gcc		# Compilador
CFLAGS = -O0	# Flags

# Ejecutables a generar
EXEC = test_backtracking test_dinamico test_goloso
all: $(EXEC)

# LDFLAGS = -Wl,--stack,1073741824	# Flags para aumentar el tamaño de la pila (1 GiB)
test_backtracking: LDFLAGS += -Wl,--stack,1073741824

# Compilación de cada ejecutable .c a .exe
%: %.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Limpieza de archivos ejecutables
clean:
	-del /Q /F *.exe 2>nul
# Nota: del solo funciona en Windows. Para sistemas Unix/Linux, usar: rm -f *.exe