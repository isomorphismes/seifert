#!/usr/bin/env bash
set -Eeuo pipefail

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

native_library=${1:-"$root/build/android/libseifert.so"}
output=${2:-"$root/build/android/seifert.apk"}
abi=${ANDROID_ABI:-armeabi-v7a}

: "${ANDROID_NDK_CHECKOUT:?ANDROID_NDK_CHECKOUT must name the canonical isomorphisms/android-NDK checkout}"

packager="$ANDROID_NDK_CHECKOUT/apk/build-nativeactivity-apk.sh"
[[ -f "$packager" ]] || {
    printf 'canonical NativeActivity packager is missing: %s\n' "$packager" >&2
    exit 2
}

export ANDROID_PACKAGE_ID=${SEIFERT_PACKAGE_ID:-org.isomorphisms.seifert}
export ANDROID_VERSION_CODE=${SEIFERT_VERSION_CODE:-2}
export ANDROID_VERSION_NAME=${SEIFERT_VERSION_NAME:-0.2}
export ANDROID_MIN_SDK=${ANDROID_API:-21}
export ANDROID_TARGET_SDK=${SEIFERT_TARGET_SDK:-34}

export ANDROID_KEYSTORE=${SEIFERT_KEYSTORE:?SEIFERT_KEYSTORE is required}
export ANDROID_KEYSTORE_TYPE=${SEIFERT_KEYSTORE_TYPE:?SEIFERT_KEYSTORE_TYPE is required}
export ANDROID_KEY_ALIAS=${SEIFERT_KEY_ALIAS:?SEIFERT_KEY_ALIAS is required}
export ANDROID_STORE_PASSWORD=${SEIFERT_STORE_PASSWORD:?SEIFERT_STORE_PASSWORD is required}
export ANDROID_KEY_PASSWORD=${SEIFERT_KEY_PASSWORD:?SEIFERT_KEY_PASSWORD is required}
export ANDROID_EXPECTED_CERT_SHA256=${SEIFERT_EXPECTED_CERT_SHA256:?SEIFERT_EXPECTED_CERT_SHA256 is required}
export ANDROID_REQUIRE_NO_DEX=1
export ANDROID_SOURCE_COMMIT=${ANDROID_SOURCE_COMMIT:-$(git -C "$root" rev-parse HEAD)}

exec bash "$packager"     "$root/android/AndroidManifest.xml"     "$native_library"     "$abi"     "$output"
