# Titre de l'application et cible de l'output
TITLE_ID = VITA2DTST
TARGET_NAME = Farmland
TARGET_DIR = out
TARGET = $(TARGET_DIR)/$(TARGET_NAME)
OBJS     = main.o player.o map.o console.o utils.o rect.o polygon.o tree.o dialogue.o box.o $(ASSETS_OBJS)

# Configuration du compilateur et des flags
PREFIX   = arm-vita-eabi
CXX      = $(PREFIX)-g++ -std=c++17
CFLAGS   = -Wl,-q -Wall -fno-lto
ASFLAGS  = $(CFLAGS)

# Bibliothèques nécessaires pour Vita et tmxlite
LIBS = -lvita2d -lSceDisplay_stub -lSceGxm_stub \
	-lSceSysmodule_stub -lSceCtrl_stub -lScePgf_stub -lScePvf_stub \
	-lSceCommonDialog_stub -lfreetype -lpng -ljpeg -lz -lm -lc -lSceAppMgr_stub \
	-ltmxlite -lSceTouch_stub

# Chercher tous les fichiers PNG dans le dossier assets de manière récursive
ASSETS_PNGS := $(shell find assets -type f -name '*.png')
# Convertir chaque fichier PNG en un fichier objet binaire .o avec la structure des sous-dossiers préservée
ASSETS_OBJS := $(ASSETS_PNGS:.png=.o)

# Ajouter les chemins pour les includes et les librairies de libvita2d et tmxlite
CFLAGS += -I../libvita2d/include -I/usr/local/include/tmxlite -Iinclude/ 
LIBS   += -L../libvita2d -L/usr/local/lib -ltmxlite -Linclude -lbz2

# Commande principale pour tout générer
all: check_out_dir $(TARGET).vpk
	@rm -f $(OBJS) eboot.bin param.sfo

check_out_dir:
	@if [ ! -d "$(TARGET_DIR)" ]; then mkdir -p $(TARGET_DIR); fi

# Génération du fichier VPK
%.vpk: eboot.bin
	vita-mksfoex -d ATTRIBUTE=0x800 -s TITLE_ID=$(TITLE_ID) "$(TARGET_NAME)" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
	-a assets/map.tmx=assets/map.tmx \
	-a assets/Pixuf.ttf=assets/Pixuf.ttf \
	-a ../sce_sys=sce_sys \
	$@

# Création de eboot.bin à partir du fichier velf
eboot.bin: $(TARGET).velf
	vita-make-fself -s $< $@

# Création du fichier velf à partir du fichier elf
%.velf: %.elf
	vita-elf-create $< $@

# Compilation des fichiers objets en un fichier ELF
$(TARGET).elf: $(OBJS)
	$(CXX) $(CFLAGS) $^ $(LIBS) -o $@

# Compilation des fichiers .cpp en objets .o
%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# Compilation des fichiers .c en objets .o (pour debugScreen)
%.o: %.c
	$(PREFIX)-gcc $(CFLAGS) -c $< -o $@

# Conversion des fichiers PNG en fichiers objets binaires .o
%.o: %.png 
	@mkdir -p $(dir $@)  # Créer les sous-dossiers pour les objets compilés
	$(PREFIX)-ld -r -b binary -o $@ $<

# Nettoyage des fichiers générés
clean:
	@rm -rf $(TARGET).vpk $(TARGET).velf $(TARGET).elf $(OBJS) \
		eboot.bin param.sfo assets/*.o
	@echo "Clean complete."

# Envoi du fichier VPK à la Vita via FTP
vpksend: $(TARGET).vpk
	curl -T $(TARGET).vpk ftp://$(PSVITAIP):1337/ux0:/downloads/
	@rm -f $(OBJS) eboot.bin param.sfo
	@echo "VPK sent to Vita."

# Envoi du fichier eboot.bin à la Vita via FTP
send: eboot.bin
	curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/$(TITLE_ID)/
	@echo "Eboot.bin sent to Vita."


opti: $(ASSETS_PNGS)
	@for png_file in $(ASSETS_PNGS); do \
		echo "Optimizing $$png_file"; \
		pngcrush -brute $$png_file temp.png && mv temp.png $$png_file; \
		pngquant --quality=65-80 $$png_file --output $$png_file; \
	done