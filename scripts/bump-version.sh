#!/bin/bash
# Pre-commit hook: auto-bump the patch component of VERSION on every commit.
# Minor/major bumps stay a deliberate manual edit — if VERSION is already
# staged (someone edited it themselves), this script leaves it alone.
set -euo pipefail

repo_root="$(git rev-parse --show-toplevel)"
version_file="$repo_root/VERSION"

if git diff --cached --name-only | grep -qx "VERSION"; then
    exit 0
fi

current="$(tr -d '[:space:]' < "$version_file")"
IFS='.' read -r major minor patch <<< "$current"

if [[ -z "${major:-}" || -z "${minor:-}" || -z "${patch:-}" ]]; then
    echo "bump-version: could not parse VERSION file ('$current'), skipping auto-bump" >&2
    exit 0
fi

patch=$((patch + 1))
new="${major}.${minor}.${patch}"

echo "$new" > "$version_file"
git add "$version_file"

echo "bump-version: $current -> $new"
