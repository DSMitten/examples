// External imports
import { dirname, join, resolve } from 'path';
import { fileURLToPath } from 'url';

// Local imports
import { findPackage } from '@clientapp/scripts';

export const packageDir = resolve(dirname(fileURLToPath(import.meta.url)), '..');

export const outDir = join(packageDir, 'site');
export const libDir = join(packageDir, 'lib');
export const srcDir = join(packageDir, 'src');

export const packageJsonPath = join(packageDir, 'package.json');
const interopPackageName = join('@microsoft', 'wv2-interop');
export const interopPackageLibHash = join(findPackage(interopPackageName), 'lib', 'srcHash.txt');

export const filesToCopy = ['index.html', 'style.css'];
