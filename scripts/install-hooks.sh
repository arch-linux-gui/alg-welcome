#!/bin/bash
# Run once after cloning to install the repo's git hooks (currently just the
# VERSION patch-bump pre-commit hook).
set -euo pipefail

repo_root="$(git rev-parse --show-toplevel)"
hooks_dir="$repo_root/.git/hooks"

chmod +x "$repo_root/scripts/bump-version.sh"
ln -sf "../../scripts/bump-version.sh" "$hooks_dir/pre-commit"

echo "Installed pre-commit hook -> scripts/bump-version.sh"
