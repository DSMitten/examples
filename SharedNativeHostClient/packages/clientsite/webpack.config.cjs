const path = require('path');

module.exports = {
    entry: ['./src/index.ts'],
    devtool: 'inline-source-map',
    output: {
        filename: 'bundle.js',
        path: path.resolve(__dirname, 'site'),
    },
    resolve: {
        extensions: ['.ts', '.js'],
        modules: ['node_modules'],
    },
    module: {
        rules: [
            {
                test: /\.ts$/,
                use: 'ts-loader',
                include: [path.resolve(__dirname, 'src')],
            },
        ],
    },
    // We do not want to minimize our code.
    mode: 'production',
    optimization: {
        minimize: false,
    },
    performance: {
        hints: false,
        maxAssetSize: 512000,
        maxEntrypointSize: 512000,
    },
};
