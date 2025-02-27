// Helper functions that run nhbt.exe to perform build actions

// External imports
import { execSync } from 'child_process';
import { existsSync } from 'fs';
import { basename, resolve } from 'path';

// Local headers
import { getInteropGenPath } from './fileUtilities.js';
import { logVerbose } from './logUtilities.js';

export function generateInteropSourceFile(inputPath, outputDir) {
    if (inputPath == null || !inputPath.endsWith('.nhapi.yaml') || !existsSync(inputPath)) {
        throw new Error(`inputPath argument must contain the path to an .nhapi.yaml file.`);
    }
    if (outputDir == null || !existsSync(outputDir)) {
        throw new Error(`outputDir argument must contain the path to an output directory for generated files.`);
    }

    const moduleName = basename(inputPath, '.nhapi.yaml');
    const outputPath = resolve(outputDir, `${moduleName}.gen.ts`);
    const interopGenPath = getInteropGenPath();

    const command = `${interopGenPath} --nhapi ${inputPath} --ts ${outputPath}`;
    logVerbose(command);
    const nhbtStdout = execSync(command);
    logVerbose(nhbtStdout);
}
