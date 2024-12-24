include config.mk

SRCDIR = source
OBJDIR = build
TARGET = $(OBJDIR)/$(PROGRAM)

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

locale:
	xgettext -k_ $(SRCS) -o locale/$(PROGRAM).pot
	msginit --no-translator -l=ru_RU.UTF-8 -i locale/$(PROGRAM).pot -o locale/ru_RU.po

install:
	msgfmt locale/ru_RU.po -o /usr/local/share/locale/ru/LC_MESSAGES/$(PROGRAM).mo

.PHONY: all clean locale install