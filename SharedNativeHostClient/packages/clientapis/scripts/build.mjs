// External imports
import { execSync } from 'child_process';

// Package imports
import {
    checkSourcesOutOfDate,
    findFiles,
    generateInteropSourceFile,
    isPathInDirectoryTree,
    logError,
    logVerbose,
    makeDirIfMissing,
    writeSourceHash,
} from '@clientapp/scripts';

// Local imports
import { genDir, interopInputFiles, outDir, packageDir, packageJsonPath, srcDir } from './common.mjs';

const packageSourceFilesGenerator = findFiles(srcDir, (filePath) => !isPathInDirectoryTree(filePath, genDir));
const sourceFilePaths = [packageJsonPath, ...packageSourceFilesGenerator, ...interopInputFiles];
const [shouldBuild, sourceHash] = checkSourcesOutOfDate(sourceFilePaths, outDir);

if (shouldBuild) {
    try {
        makeDirIfMissing(genDir);

        for (const interopInputFile of interopInputFiles) {
            generateInteropSourceFile(interopInputFile, genDir);
        }

        const tscCommand = 'npx tsc --build --verbose';
        logVerbose(tscCommand);
        const tscStdout = execSync(tscCommand, {
            cwd: packageDir,
            encoding: 'UTF-8',
        });
        logVerbose(tscStdout);
 
        writeSourceHash(sourceHash, outDir);
    } catch (err) {
        logError(err);
        process.exit(-1);
    }
}
