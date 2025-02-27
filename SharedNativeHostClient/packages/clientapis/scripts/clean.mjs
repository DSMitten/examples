// External imports
import { existsSync, rmSync } from 'fs';

if (existsSync('../../node_modules/@clientapp/scripts')) {
    // Dynamic imports - don't want to run these imports unless our scripts package is installed
    // So 'zap build' commands don't error out if 'npm install' hasn't been run yet
    const common = await import('./common.mjs');

    rmSync(common.genDir, { recursive: true, force: true });
    rmSync(common.outDir, { recursive: true, force: true });
}
