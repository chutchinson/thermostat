import { Component, Host, Prop, State, Element, h } from '@stencil/core'

@Component({
    tag: 'thermostat-button-group',
    styleUrl: 'button-group.scss',
    shadow: true
})
export class ButtonGroup {
    
    public render() {
        return (
            <Host>
                <slot />
            </Host>
        )
    }

}