#!/bin/bash
#
# update-nav.sh - Update navigation in all HTML documentation files
#
# Usage: ./update-nav.sh [-n|--dry-run] [-b|--backup] [-h|--help]
#
# Options:
#   -n, --dry-run    Show what would be changed without writing files
#   -b, --backup     Create .bak backup files before updating
#   -h, --help       Show this help message
#

set -euo pipefail

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DOCS_DIR="$SCRIPT_DIR"
DRY_RUN=false
CREATE_BACKUP=false

# Explicit page order (excluding index.html)
DOCS_ORDER=(
    "packages"
    "directives"
    "deconstruct"
    "macros"
    "include"
    "fstrings"
    "tests"
    "const"
    "plugins"
)

# Show help message
show_help() {
    cat << EOF
Update navigation in all HTML documentation files

Usage: ./update-nav.sh [OPTIONS]

Options:
  -n, --dry-run    Show what would be changed without writing files
  -b, --backup     Create .bak backup files before updating
  -h, --help       Show this help message

Description:
  This script automatically generates navigation for all HTML files in the
  docs/ directory. It extracts page titles from H1 tags and creates a
  consistent navigation dropdown across all documentation pages.

Examples:
  ./update-nav.sh --dry-run    # Preview changes
  ./update-nav.sh --backup     # Update with backups
  ./update-nav.sh              # Standard update

EOF
}

# Parse command line arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -n|--dry-run)
                DRY_RUN=true
                shift
                ;;
            -b|--backup)
                CREATE_BACKUP=true
                shift
                ;;
            -h|--help)
                show_help
                exit 0
                ;;
            *)
                echo "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done
}

# Extract H1 title from HTML file
extract_h1_title() {
    local file="$1"
    local title

    # Extract text between <h1> and </h1>
    title=$(grep -oP '(?<=<h1>).*?(?=</h1>)' "$file" 2>/dev/null | head -n1 | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

    # Fallback to filename if no H1 found
    if [ -z "$title" ]; then
        local basename=$(basename "$file" .html)
        # Capitalize first letter
        title="$(tr '[:lower:]' '[:upper:]' <<< ${basename:0:1})${basename:1}"
        echo "WARNING: No H1 in $file, using filename: $title" >&2
    fi

    echo "$title"
}

# Build navigation dropdown HTML
build_nav_dropdown() {
    local nav_html="      <nav>
        <ul>
          <li>
            <strong><a href=\"/\">Builder</a></strong>
          </li>
        </ul>
        <ul>
          <li>
            <details class=\"dropdown\">
              <summary>Docs</summary>
              <ul dir=\"rtl\">"

    # Add each page in specified order
    for page in "${DOCS_ORDER[@]}"; do
        local file="$DOCS_DIR/${page}.html"

        if [ ! -f "$file" ]; then
            echo "WARNING: File not found: $file" >&2
            continue
        fi

        local title=$(extract_h1_title "$file")
        local url="/$page"

        nav_html="$nav_html
                <li><a href=\"$url\">$title</a></li>"
    done

    nav_html="$nav_html
              </ul>
            </details>
          </li>
        </ul>
      </nav>"

    echo "$nav_html"
}

# Replace nav block in file
replace_nav_in_file() {
    local file="$1"
    local new_nav="$2"
    local tmpfile="${file}.tmp"

    # Check if nav block exists
    if ! grep -q '<nav>' "$file"; then
        echo "WARNING: No <nav> tag found in $file, skipping" >&2
        return 1
    fi

    # Create backup if requested
    if $CREATE_BACKUP; then
        cp "$file" "${file}.bak"
    fi

    # Use awk to replace nav block
    awk -v nav="$new_nav" '
        BEGIN { in_nav=0; printed_nav=0 }
        /<nav>/ {
            if (!printed_nav) {
                print nav
                printed_nav=1
            }
            in_nav=1
            next
        }
        /<\/nav>/ {
            in_nav=0
            next
        }
        !in_nav { print }
    ' "$file" > "$tmpfile"

    # Verify temp file
    if [ ! -s "$tmpfile" ]; then
        echo "ERROR: Failed to update $file - empty output" >&2
        rm -f "$tmpfile"
        return 1
    fi

    if ! grep -q '<nav>' "$tmpfile"; then
        echo "ERROR: Failed to update $file - nav block missing in output" >&2
        rm -f "$tmpfile"
        return 1
    fi

    # Replace original file
    if $DRY_RUN; then
        echo "Would update: $file"
        rm -f "$tmpfile"
    else
        mv "$tmpfile" "$file"
        echo "Updated: $file"
    fi

    return 0
}

# Main execution
main() {
    parse_args "$@"

    echo "Generating navigation for HTML documentation..."
    echo ""

    # Validate docs directory
    if [ ! -d "$DOCS_DIR" ]; then
        echo "ERROR: Directory not found: $DOCS_DIR"
        exit 1
    fi

    # Build navigation HTML
    local nav_html=$(build_nav_dropdown)

    if $DRY_RUN; then
        echo "Generated navigation:"
        echo "$nav_html"
        echo ""
    fi

    # Update all HTML files
    local files_updated=0
    local files_failed=0

    for html_file in "$DOCS_DIR"/*.html; do
        if [ ! -f "$html_file" ]; then
            continue
        fi

        if replace_nav_in_file "$html_file" "$nav_html"; then
            files_updated=$((files_updated + 1))
        else
            files_failed=$((files_failed + 1))
        fi
    done

    # Summary
    echo ""
    echo "Summary:"
    echo "  Files updated: $files_updated"
    echo "  Files failed: $files_failed"

    if $DRY_RUN; then
        echo "  (Dry run - no files were modified)"
    fi

    if $CREATE_BACKUP; then
        echo "  (Backups created with .bak extension)"
    fi

    if [ $files_failed -gt 0 ]; then
        exit 1
    fi
}

# Run main with all arguments
main "$@"
