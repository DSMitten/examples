// Determines if a command-line argument is present or not
export function isArgPresent(argName) {
    for (let argIndex = 1; argIndex < process.argv.length; argIndex++) {
        if (process.argv[argIndex] == argName) {
            return true;
        }
    }
    return false;
}

// Gets a single value following a command-line argument, or undefined if the argument does not appear
export function tryGetArgValue(argName) {
    for (let argIndex = 1; argIndex < process.argv.length; argIndex++) {
        if (process.argv[argIndex] == argName) {
            argIndex++;
            if (argIndex < process.argv.length) {
                return process.argv[argIndex];
            }
        }
    }
    return undefined;
}
