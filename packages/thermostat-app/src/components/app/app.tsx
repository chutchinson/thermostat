import { Component, Host, Prop, State, Element, h, Listen } from '@stencil/core'
import { ThermostatClient, ThermostatFanMode, ThermostatMode } from '../../client/thermostat'
import { farhenheit } from '../../utils'

@Component({
    tag: 'thermostat-app',
    styleUrl: 'app.scss',
    shadow: true
})
export class App {
    
    private thermostat = new ThermostatClient()

    @Prop({ reflectToAttr: true, mutable: true }) public state: 'cool' | 'heat' | 'idle' = 'idle'

    @State() private location: string = 'Home'
    @State() private temperatureTarget: number | null = 22.77778
    @State() private temperature: number | null = null
    @State() private humidity: number | null = null
    @State() private loading: boolean = false
    @State() private auto: boolean = false
    @State() private fan: boolean = false

    public componentDidLoad() {
        window.setInterval(this.update.bind(this), 10000)
        this.update()
    }

    private async update() {
        const report = await this.thermostat.report()
        this.state = report.data.output || 'idle'
        this.fan = report.data.fan_mode === 'on'
        this.auto = report.data.mode === 'automatic'
        this.temperatureTarget = report.data.temperatureTarget || null
        this.temperature = report.data.temperature || null
        this.humidity = report.data.humidity || null
    }

    private get icon() {
        switch (this.state) {
            case 'cool': return 'snowflake'
            case 'heat': return 'fire'
            case 'idle': return 'check'
        }
    }

    public render() {
        const { location, statusText, icon } = this
        const temperature = this.temperature !== null
            ? <span class="temperature">{farhenheit(this.temperature).toFixed(0)}&deg;</span>
            : <span class="temperature">--</span>
        const humidity = this.humidity !== null
            ? this.humidity.toFixed(0) + '%'
            : '--'
        return (
            <Host>
                <section class="status">
                    <header>
                        <h1>{location}</h1>
                    </header>
                    <div class="status-wrapper">
                        <thermostat-icon class="icon" glyph={icon} />
                        <span class="state">{statusText}</span>
                        { temperature }
                        <span class="humidity">
                            <thermostat-icon glyph="tint" />
                            { humidity }
                        </span>
                    </div>
                </section>
                <section class="controls">
                    <div class="controls__left">
                        <span>Fan</span>
                        <thermostat-button-group>
                            <thermostat-button size="icon" checked={this.fan} onClick={this.handleClickFan}>
                                <thermostat-icon glyph="fan" />
                            </thermostat-button>
                        </thermostat-button-group>
                    </div>
                    <div class="controls__temperature">
                        <thermostat-button-group>
                            <thermostat-button size="icon" onClick={this.handleTemperatureUp}>
                                <thermostat-icon glyph="angle-up" />
                            </thermostat-button>
                            <thermostat-button size="icon" onClick={this.handleTemperatureDown}>
                                <thermostat-icon glyph="angle-down" />
                            </thermostat-button>
                        </thermostat-button-group>
                    </div>
                    <div class="controls__right">
                        <span>Auto</span>
                        <thermostat-button-group>
                            <thermostat-button size="icon" checked={this.auto} onClick={this.handleClickAuto}>
                                <thermostat-icon glyph="power-off" />
                            </thermostat-button>
                        </thermostat-button-group>
                    </div>
                </section>
            </Host>
        )
    }

    private get statusText() {
        const target = this.temperatureTarget !== null
            ? farhenheit(this.temperatureTarget).toFixed(0) + '°'
            : '???'
        switch (this.state) {
            case 'heat': return `heating to ${target}`
            case 'cool': return `cooling to ${target}`
            default: return `idle at ${target}`
        }
    }

    private handleTemperatureUp = async () => {
        if (!this.temperatureTarget) {
            return
        }
        this.temperatureTarget += 0.5556
        this.temperatureTarget = Math.min(this.temperatureTarget, 29.444444444)
        await this.thermostat.setTemperatureTarget(this.temperatureTarget)
    }

    private handleTemperatureDown = async () => {
        if (!this.temperatureTarget) {
            return
        }
        this.temperatureTarget -= 0.5556
        this.temperatureTarget = Math.max(15.555555556, this.temperatureTarget)
        await this.thermostat.setTemperatureTarget(this.temperatureTarget)
    }

    private handleClickFan = async () => {
        this.fan = !this.fan
        const mode: ThermostatFanMode = this.fan ? 'on' : 'automatic'
        await this.thermostat.setFanMode(mode)
    }

    private handleClickAuto = async () => {
        this.auto = !this.auto
        const mode: ThermostatMode = this.auto ? 'automatic': 'off'
        await this.thermostat.setMode(mode)
    }

}