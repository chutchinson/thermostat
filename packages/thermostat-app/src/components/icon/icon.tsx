import { Component, Host, Prop, State, Element, h } from '@stencil/core'
import { IconStyle } from './interface'

const styles = {
    'solid': 'fas',
    'regular': 'far',
    'brands': 'fab'
}

@Component({
    tag: 'thermostat-icon',
    styleUrl: 'icon.scss',
    shadow: true
})
export class Icon {

    @Prop() public glyph: string = 'fire'
    
    public render() {
        const href = `assets/icons.svg#fa-${this.glyph}`
        return (
            <Host>
                <svg>
                    <use xlinkHref={href} />
                </svg>
            </Host>
        )
    }

}