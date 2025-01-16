MAKEFLAGS += --silent -j6
CC := clang

SRCDIR  := ./src
BINDIR  := ./bin
INCDIR  := ./include
LIBDIR  := ./lib
OBJDIR  := $(BINDIR)/obj

CFILES := $(foreach dir,$(SRCDIR), $(wildcard $(dir)/*.c) $(wildcard $(dir)/**/*.c) )
OBJS   := $(CFILES:./%.c=$(OBJDIR)/%.o)
DFILES := $(CFILES:./%.c=$(OBJDIR)/%.d)

INC_DIRS:= $(foreach dir, $(SRCDIR), -I$(dir)) -I$(INCDIR)
LIB_DIRS:= -L$(LIBDIR) 
LDFLAGS := -g3 -glldb -lkernel32 
CFLAGS :=  -O0 -g3 -glldb -Wall -Wextra -Werror -Wno-unused-variable -std=c23
DFLAG  := -MMD -MP

PROJECT_NAME := test_project
BINARY := $(BINDIR)/$(PROJECT_NAME).exe


run: compile
	@powershell -ExecutionPolicy Bypass -Command "$(BINARY)" 
compile: $(OBJS)
	@powershell -ExecutionPolicy Bypass -Command "$(CC)  $^ -o $(BINARY) $(LIB_DIRS) $(LDFLAGS)"


$(OBJDIR)/%.o: %.c
	@powershell New-Item -ItemType Directory -Path '$(subst /,\,$(@D))' -Force
	@powershell -ExecutionPolicy Bypass -Command "$(CC) $(CFLAGS) -c $< $(DFLAG) -o $@ $(INC_DIRS)"

clean:
	@powershell -NoProfile -ExecutionPolicy Bypass -Command " \
		Write-Output '⬤ Cleaning Main'; \
		"
	@powershell -NoProfile -ExecutionPolicy Bypass -Command "\
		if (Test-Path -Path "$(OBJDIR)" -PathType Container) { \
			Remove-Item -Path '$(OBJDIR)\\*.o' -Recurse -Force; \
			Remove-Item -Path "$(OBJDIR)" -Recurse -Force; \
		} else { \
			Write-Output '	⚠ Directory $(OBJDIR) is missing - nothing to clean'; \
		}"
	@powershell -NoProfile -ExecutionPolicy Bypass -Command "\
		if(Test-Path -Path '$(BINDIR)/$(PROJECT_NAME).exe' -PathType Leaf){ \
			Remove-Item -Path "$(BINDIR)/*.exe" -Force;\
		} else { \
			Write-Output '	⚠ $(OBJDIR)/$(PROJECT_NAME).exe missing - nothing to clean'; \
		}"

scafold:
	@powershell -ExecutionPolicy Bypass -Command " $$folderName = '$(SRCDIR)'; if (-not (Test-Path $$folderName)){ New-Item -ItemType Directory -Path $$folderName } else{Write-Host "Folder '$$folderName' OK"}"
	@powershell -ExecutionPolicy Bypass -Command " $$folderName = '$(BINDIR)'; if (-not (Test-Path $$folderName)){ New-Item -ItemType Directory -Path $$folderName } else{Write-Host "Folder '$$folderName' OK"}"
	@powershell -ExecutionPolicy Bypass -Command " $$folderName = '$(INCDIR)'; if (-not (Test-Path $$folderName)){ New-Item -ItemType Directory -Path $$folderName } else{Write-Host "Folder '$$folderName' OK"}"
	@powershell -ExecutionPolicy Bypass -Command " $$folderName = '$(LIBDIR)'; if (-not (Test-Path $$folderName)){ New-Item -ItemType Directory -Path $$folderName } else{Write-Host "Folder '$$folderName' OK"}"
ifeq ($(wildcard $(SRCDIR)/main.c),)
	@echo "$(SRCDIR)/main.c > not present! Creating Deafault"
	@powershell -ExecutionPolicy Bypass -Command " $$mainFile = '$(SRCDIR)/main.c'; $$projectName='$(PROJECT_NAME)';  New-Item -ItemType File -Path $$mainFile -Force; Add-Content -Path $$mainFile -Value '#include <stdio.h>'; Add-Content -Path $$mainFile -Value ''; Add-Content -Path $$mainFile -Value 'int main() {'; Add-Content -Path $$mainFile -Value '    printf(\"Hello  World\");'; Add-Content -Path $$mainFile -Value '    return 0;'; Add-Content -Path $$mainFile -Value '}'"
else
	@echo "$(SRCDIR)/main.c > OK
endif

-include $(DFILES)
