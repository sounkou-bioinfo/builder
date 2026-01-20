# Installation

## Quick Install

```bash
curl -fsSL https://builder.opifex.org/install.sh | sh
```

To install to a custom location:

```bash
PREFIX=~/.local curl -fsSL https://builder.opifex.org/install.sh | sh
```

## Manual Install

```bash
git clone https://github.com/devOpifex/builder.git --depth 1
cd builder
sudo make install
```

To install to a custom location:

```bash
make install PREFIX=~/.local
```

