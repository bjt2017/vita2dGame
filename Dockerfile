# Utiliser une image de base Ubuntu
FROM ubuntu:22.04

# Définir des variables d'environnement
ENV VITASDK=/usr/local/vitasdk
ENV PATH=$VITASDK/bin:$PATH
ENV PSVITAIP=192.168.1.38

# Mettre à jour les packages et installer les dépendances nécessaires
RUN apt-get update && apt-get install -y \
    git \
    wget \
    build-essential \
    cmake \
    python3 \
    zlib1g-dev \
    libzstd-dev \
    curl \
    sudo \
    meson \
    ninja-build \
    libpugixml-dev \
    qemu qemu-user-static binfmt-support \
    pkg-config

# Activer QEMU pour les binaires multi-architecture
RUN update-binfmts --enable qemu-x86_64

# Cloner et installer VitaSDK
RUN git clone https://github.com/vitasdk/vdpm.git /vdpm && \
    cd /vdpm && \
    ./bootstrap-vitasdk.sh && \
    ./install-all.sh

# Cloner et configurer tmxlite
RUN git clone https://github.com/fallahn/tmxlite.git /tmxlite

# Compiler et installer tmxlite pour Vita SDK
RUN cd /tmxlite/tmxlite && mkdir build && cd build && \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake && \
    make -j$(nproc) && \
    make install

# Définir le répertoire de travail
WORKDIR /app

# Copier le contenu du projet dans le conteneur
COPY . /app

# Créer un répertoire de build
RUN mkdir -p build

# Commande par défaut
CMD ["bash"]
