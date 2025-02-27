// External imports
import { existsSync, rmSync } from 'fs';

if (existsSync('../../node_modules/@clientapp/scripts')) {
    // Dynamic import runs after the wv2-scripts package check
    const common = await import('./common.mjs');

    rmSync(common.libDir, { recursive: true, force: true });
    rmSync(common.outDir, { recursive: true, force: true });
}
