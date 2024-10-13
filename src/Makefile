TITLE_ID = VITA2DTST
TARGET   = out/vita2dsample
OBJS     = main.o $(ASSETS_OBJS)

PREFIX  = arm-vita-eabi
CXX     = $(PREFIX)-g++
CFLAGS  = -Wl,-q -Wall -fno-lto
ASFLAGS = $(CFLAGS)

# Librairies Vita et tmxlite
LIBS = -lvita2d -lSceDisplay_stub -lSceGxm_stub \
	-lSceSysmodule_stub -lSceCtrl_stub -lScePgf_stub -lScePvf_stub \
	-lSceCommonDialog_stub -lfreetype -lpng -ljpeg -lz -lm -lc -lSceAppMgr_stub \
	-ltmxlite

# Trouver toutes les images PNG dans le dossier assets
ASSETS_PNGS := $(wildcard assets/*.png)
# Convertir chaque fichier PNG en un fichier objet .o
ASSETS_OBJS := $(ASSETS_PNGS:.png=.o)

# Ajouter les chemins de libvita2d et tmxlite
CFLAGS += -I../libvita2d/include -I/usr/local/include/tmxlite
LIBS += -L../libvita2d -L/usr/local/lib -ltmxlite

all: $(TARGET).vpk
	@rm -f $(OBJS) eboot.bin param.sfo

# Génération du VPK avec la map
%.vpk: eboot.bin
	vita-mksfoex -d ATTRIBUTE=0x800 -s TITLE_ID=$(TITLE_ID) "$(TARGET)" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin -a assets/map.tmx=assets/map.tmx $@

# Création de eboot.bin à partir de velf
eboot.bin: $(TARGET).velf
	vita-make-fself -s $< $@

# Création de velf à partir de elf
%.velf: %.elf
	vita-elf-create $< $@

# Compilation des fichiers objets en elf
$(TARGET).elf: $(OBJS)
	$(CXX) $(CFLAGS) $^ $(LIBS) -o $@

# Règle pour compiler les fichiers C++ (.cpp) en fichiers objets (.o)
%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# Règle pour convertir chaque fichier PNG en un fichier objet binaire
%.o: %.png 
	$(PREFIX)-ld -r -b binary -o $@ $<

	
	


clean:
	@rm -rf $(TARGET).vpk $(TARGET).velf $(TARGET).elf $(OBJS) \
		eboot.bin param.sfo assets/map.o

vpksend: $(TARGET).vpk
	curl -T $(TARGET).vpk ftp://$(PSVITAIP):1337/ux0:/
	@rm -f $(OBJS) eboot.bin param.sfo
	@echo "Sent."

send: eboot.bin
	curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/$(TITLE_ID)/
	@echo "Sent."
