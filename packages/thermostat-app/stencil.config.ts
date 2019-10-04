import { Config } from '@stencil/core'
import { sass } from '@stencil/sass'

export const config: Config = {
    namespace: 'thermostat',
    globalStyle: 'src/global/index.scss',
    plugins: [
        sass({
            injectGlobalPaths: [
                'src/global/variables.scss',
                'src/global/mixins.scss'
            ]
        })
    ]
}