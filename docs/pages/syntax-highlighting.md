---
title: Syntax Highlighting
---

# Syntax Highlighting

Builder directives start with `#> ` which editors treat as comments by default. Here's how to add custom highlighting for directives in popular editors.

## Neovim (TreeSitter)

Create a query file to extend R syntax highlighting:

**`~/.config/nvim/queries/r/highlights.scm`**

```scheme
;extends

((comment) @keyword.directive
  (#lua-match? @keyword.directive "^#>")
  (#set! priority 101))
```

Then add a custom highlight in your config:

**`init.lua`**

```lua
vim.api.nvim_set_hl(0, "@keyword.directive.r", { fg = "#c792ea", bold = true })
```

## Vim (Legacy Syntax)

Add to `~/.vim/after/syntax/r.vim`:

```vim
syn match rBuilderDirective "^#> .*$"
hi def link rBuilderDirective PreProc
```

## Emacs (ESS)

Add to your `init.el`:

```elisp
(add-hook 'ess-r-mode-hook
  (lambda ()
    (font-lock-add-keywords nil
      '(("^#> .*$" . font-lock-preprocessor-face)))))
```
