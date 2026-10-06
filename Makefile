# ============================================================
#  Makefile — Smart Hospital Management System
#  Compiler: GCC | Standard: C11
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude -g
TARGET  = hospital
SRCDIR  = src
OBJDIR  = obj

# ── Source and object files ──────────────────────────────────
SRCS = $(SRCDIR)/main.c       \
       $(SRCDIR)/utility.c    \
       $(SRCDIR)/login.c      \
       $(SRCDIR)/patient.c    \
       $(SRCDIR)/doctor.c     \
       $(SRCDIR)/appointment.c\
       $(SRCDIR)/emergency.c  \
       $(SRCDIR)/ambulance.c  \
       $(SRCDIR)/billing.c    \
       $(SRCDIR)/bed.c        \
       $(SRCDIR)/linked_list.c

OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# ── Default target ───────────────────────────────────────────
all: dirs $(TARGET)
	@echo ""
	@echo "  ✔  Build successful! Run with: ./$(TARGET)"
	@echo ""

# ── Link ─────────────────────────────────────────────────────
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ── Compile each source ──────────────────────────────────────
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ── Create required directories ──────────────────────────────
dirs:
	@mkdir -p $(OBJDIR) data logs backup

# ── Clean build artifacts ────────────────────────────────────
clean:
	rm -rf $(OBJDIR) $(TARGET)
	@echo "  ✔  Cleaned."

# ── Clean everything including data ──────────────────────────
cleanall: clean
	rm -rf data logs backup
	@echo "  ✔  Full clean done."

# ── Run ──────────────────────────────────────────────────────
run: all
	./$(TARGET)

# ── Rebuild from scratch ─────────────────────────────────────
rebuild: clean all

.PHONY: all clean cleanall run rebuild dirs
