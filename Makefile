# Tempraroly Makefile for development, the relase will use GNU autotools

TARGET=gummi
OBJS = main.o gui.o preview.o motion.o editor.o

CFLAGS=-g -Wall -export-dynamic `pkg-config --cflags --libs gtk+-2.0 gtksourceview-2.0 cairo poppler-glib`

gummi: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJS)
