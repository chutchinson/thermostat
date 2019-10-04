import { Component, Host, Prop, State, Element, h } from '@stencil/core'
import { ButtonSize } from './interface'

@Component({
    tag: 'thermostat-button',
    styleUrl: 'button.scss',
    shadow: true
})
export class Button {
    
    @Prop({ reflectToAttr: true }) public size: ButtonSize = 'auto'
    @Prop({ reflectToAttr: true }) public disabled: boolean = false
    @Prop({ reflectToAttr: true }) public checked: boolean = false

    public render() {
        return (
            <Host>
                <div class="wrapper">
                    <slot name="start" />
                    <slot />
                    <slot name="end" />
                </div>
            </Host>
        )
    }

}