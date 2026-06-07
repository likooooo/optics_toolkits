#!/usr/bin/env bash
# Configure ~/.ssh for GitHub deploy key in CI (supports ed25519 and RSA).
set -euo pipefail

if [ -z "${MY_PRIVATE_KEY_1:-}" ]; then
    echo "MY_PRIVATE_KEY_1 is empty" >&2
    exit 1
fi

mkdir -p ~/.ssh
printf '%s\n' "${MY_PRIVATE_KEY_1}" > ~/.ssh/ci_key
chmod 600 ~/.ssh/ci_key

if grep -q "BEGIN RSA PRIVATE KEY" ~/.ssh/ci_key; then
    mv ~/.ssh/ci_key ~/.ssh/id_rsa
    KEY=~/.ssh/id_rsa
else
    mv ~/.ssh/ci_key ~/.ssh/id_ed25519
    KEY=~/.ssh/id_ed25519
fi

ssh-keyscan -t rsa,ed25519 github.com >> ~/.ssh/known_hosts 2>/dev/null

{
    echo "Host github.com"
    echo "  IdentityFile ${KEY}"
    echo "  IdentitiesOnly yes"
} >> ~/.ssh/config
chmod 600 ~/.ssh/config

echo "GIT_SSH_COMMAND=ssh -F ${HOME}/.ssh/config" >> "${GITHUB_ENV}"
export GIT_SSH_COMMAND="ssh -F ${HOME}/.ssh/config"

ssh -T git@github.com 2>&1 | tee /tmp/ssh-test.txt || true
grep -qi "successfully authenticated" /tmp/ssh-test.txt && echo "SSH to GitHub OK"
