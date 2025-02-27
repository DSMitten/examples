// External imports
import { execSync } from 'child_process';
import { resolve } from 'path';

// Package imports
import {
    callWebpack,
    checkSourcesOutOfDate,
    copyFileIfOutOfDate,
    findFiles,
    logError,
    logVerbose,
    makeDirIfMissing,
    writeSourceHash,
} from '@clientapp/scripts';

// Local imports
import { filesToCopy, libDir, outDir, packageDir, packageJsonPath, srcDir, interopPackageLibHash } from './common.mjs';
import webpackConfig from '../webpack.config.cjs';

const sourceFilePaths = [packageJsonPath, ...findFiles(srcDir), interopPackageLibHash];
const [shouldBuild, sourceHash] = checkSourcesOutOfDate(sourceFilePaths, libDir);
if (shouldBuild) {
    try {
        if (!process.env.GITHUB_ACTIONS) {
            logVerbose("npx prettier --cache-location=.prettiercache --write 'src/**/*.ts'");
            const prettierStdout = execSync('npx prettier --cache-location=.prettiercache --write src/**/*.ts', {
                cwd: packageDir,
                encoding: 'UTF-8',
            });

            logVerbose(prettierStdout);
        }

        makeDirIfMissing(outDir);

        logVerbose('npx tsc');

        const tsxStdout = execSync('npx tsc --build --verbose', {
            cwd: packageDir,
            encoding: 'UTF-8',
        });

        logVerbose(tsxStdout);

        for (const fileToCopy of filesToCopy) {
            const srcPath = resolve(srcDir, fileToCopy);
            const destPath = resolve(outDir, fileToCopy);

            copyFileIfOutOfDate(srcPath, destPath);
        }

        await callWebpack(webpackConfig, packageDir);

        writeSourceHash(sourceHash, libDir);
    } catch (err) {
        logError(err);
        process.exit(-1);
    }
}
