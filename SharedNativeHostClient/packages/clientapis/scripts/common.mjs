// External imports
import { dirname, join, resolve } from 'path';
import { fileURLToPath } from 'url';

// Local imports
import { getCppSourceDir } from '@clientapp/scripts';

export const packageDir = resolve(dirname(fileURLToPath(import.meta.url)), '..');

export const outDir = join(packageDir, 'lib');
export const srcDir = join(packageDir, 'src');
export const genDir = join(srcDir, 'gen');

export const packageJsonPath = join(packageDir, 'package.json');

export const interopInputFiles = [
    join(getCppSourceDir(), 'clientApp', 'OSVersion.nhapi.yaml'),
];
