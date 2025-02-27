// External imports
import { copyFileSync, existsSync, mkdirSync, readdirSync, readFileSync, statSync, writeFileSync } from 'fs';
import { dirname, isAbsolute, join, relative } from 'path';
import { createHash } from 'crypto';
import { fileURLToPath } from 'url';

// Local imports
import { tryGetArgValue } from './argUtilities.js';
import { logMessage, logSilly, logVerbose } from './logUtilities.js';
import { isWindows, isMacOS, isLinux } from './platformUtilities.js';

export function makeDirIfMissing(dirPath) {
    if (!existsSync(dirPath)) {
        logVerbose(`Creating directory '${dirPath}'`);
        mkdirSync(dirPath);
    }
}

export function copyFileIfOutOfDate(srcPath, destPath) {
    const srcStat = statSync(srcPath);
    const destStat = statSync(destPath, { throwIfNoEntry: false });

    let shouldCopy = true;
    if (destStat === undefined) {
        logSilly(`File '${destPath}' does not exist - should copy`);
    } else if (destStat.mtime < srcStat.mtime) {
        logSilly(`File '${srcPath}' has changed - should copy`);
    } else {
        shouldCopy = false;
    }

    if (shouldCopy) {
        logVerbose(`Copying '${srcPath}' to '${destPath}'`);
        copyFileSync(srcPath, destPath);
    }
}

export function getRepoRootDir() {
    const repoRootDir = join(dirname(fileURLToPath(import.meta.url)), '../../..');
    return repoRootDir;
}

export function getCppSourceDir() {
    const cppSourceDir = join(getRepoRootDir(), 'dev', 'src');
    return cppSourceDir;
}

export function getInteropGenPath() {
    // If path to interopgen was provided on the command-line, use it
    const interopGenPathArg = tryGetArgValue('--interopgen');
    if (interopGenPathArg) {
        return interopGenPathArg;
    }

    // Otherwise, try to find it
    let interopgenName = isWindows() ? 'interopgen.exe' : 'interopgen';
    let targetTriplets = [];
    let hostTriplet = '';
    if (isWindows()) {
        targetTriplets = ['x86-windows', 'x64-windows', 'arm64-windows'];
        hostTriplet = 'x64-windows';
    } else if (isMacOS() && process.arch === 'x64') {
        targetTriplets = ['x64-osx'];
        hostTriplet = 'x64-osx';
    } else if (isMacOS() && process.arch === 'arm64') {
        targetTriplets = ['arm64-osx'];
        hostTriplet = 'x64-osx';
    } else if (isLinux()) {
        targetTriplets = ['x64-linux'];
        hostTriplet = 'x64-linux';
    } else {
        throw new Error('Unsupported config');
    }

    for (const targetTriplet of targetTriplets) {
        const interopGenPath = join(
            getRepoRootDir(),
            'external',
            'vcpkg',
            `installed-${targetTriplet}`,
            hostTriplet,
            'tools',
            'ms-wv2host-tools',
            interopgenName
        );

        if (existsSync(interopGenPath)) {
            return interopGenPath;
        }
    }

    throw new Error('Could not find interopgen - has vcpkg install happened?');
}

// Gets the list of files in the specified directory
export function* findFiles(directoryName, filter = null) {
    const directoryEntries = readdirSync(directoryName, { withFileTypes: true });
    for (const directoryEntry of directoryEntries) {
        const fullPath = join(directoryName, directoryEntry.name);

        if (directoryEntry.isFile()) {
            if (filter != null) {
                if (filter(fullPath)) {
                    yield fullPath;
                }
            } else {
                yield fullPath;
            }
        } else if (directoryEntry.isDirectory()) {
            yield* findFiles(fullPath, filter);
        }
    }
}

export function isPathInDirectoryTree(path, dir) {
    const relativePath = relative(dir, path);
    return relativePath != null && !relativePath.startsWith('..') && !isAbsolute(relativePath);
}

export function findPackage(packageName) {
    const packagePath = join(getRepoRootDir(), 'node_modules', packageName);
    return packagePath;
}

// Construct a string from the modification date, the filename and the filesize
// of each file in an array of files
function computeFilesHash(filePaths) {
    if (!Array.isArray(filePaths)) {
        throw new Error('filePaths is not an array');
    }
    const hash = createHash('sha256');

    for (const filePath of filePaths) {
        const statInfo = statSync(filePath);
        // compute hash string name:size:mtime
        const fileInfo = `${filePath}:${statInfo.size}:${statInfo.mtimeMs}`;
        hash.update(fileInfo);
    }

    return hash.digest('hex');
}

function getSourceHashPath(outDir) {
    return join(outDir, 'srcHash.txt');
}

export function checkSourcesOutOfDate(sourceFilePaths, outDir) {
    const sourceHash = computeFilesHash(sourceFilePaths);
    const srcHashPath = getSourceHashPath(outDir);

    let shouldBuild = true;
    if (existsSync(srcHashPath)) {
        const existingSourcehash = readFileSync(srcHashPath, { encoding: 'UTF-8' });
        shouldBuild = existingSourcehash !== sourceHash;
    }

    logMessage(`should build?: ${shouldBuild} - sourceHash: ${sourceHash}`);
    return [shouldBuild, sourceHash];
}

export function writeSourceHash(sourceHash, outDir) {
    const srcHashPath = getSourceHashPath(outDir);
    writeFileSync(srcHashPath, sourceHash, { encoding: 'UTF-8' });
}
