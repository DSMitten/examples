// External imports
import { platform } from 'os';

export function isWindows() {
    return platform() === 'win32';
}

export function isMacOS() {
    return platform() === 'darwin';
}

export function isLinux() {
    return platform() === 'linux';
}
