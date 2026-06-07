#!/usr/bin/env bash
# Configure ~/.ssh for GitHub deploy key in CI (supports ed25519 and RSA).
set -uo pipefail

KEY_MATERIAL="${MY_PRIVATE_KEY_1:-}"
if [ -z "${KEY_MATERIAL}" ] && [ -n "${MY_PRIVATE_KEY_1_VAR:-}" ]; then
    KEY_MATERIAL="${MY_PRIVATE_KEY_1_VAR}"
fi

if [ -z "${KEY_MATERIAL}" ]; then
    echo "MY_PRIVATE_KEY_1 is empty (check repository secret or variable)" >&2
    exit 1
fi

mkdir -p ~/.ssh
umask 077
printf '%s' "${KEY_MATERIAL}" > ~/.ssh/ci_key
# Ensure PEM ends with a newline
[ "$(tail -c1 ~/.ssh/ci_key | wc -l)" -eq 0 ] && echo >> ~/.ssh/ci_key

if grep -q "BEGIN RSA PRIVATE KEY" ~/.ssh/ci_key; then
    mv ~/.ssh/ci_key ~/.ssh/id_rsa
    KEY=~/.ssh/id_rsa
else
    mv ~/.ssh/ci_key ~/.ssh/id_ed25519
    KEY=~/.ssh/id_ed25519
fi
chmod 600 "${KEY}"

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
if grep -qi "successfully authenticated" /tmp/ssh-test.txt; then
    echo "SSH to GitHub OK"
else
    echo "SSH test output:"
    cat /tmp/ssh-test.txt
    exit 1
fi
