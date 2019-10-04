import axios, { AxiosInstance } from 'axios'

export type ThermostatOutput = 'idle' | 'heat' | 'cool'
export type ThermostatFanMode = 'automatic' | 'on'
export type ThermostatMode = 'automatic' | 'off'

export interface ThermostatReport {
    cmd: 'report',
    data: {
        output: ThermostatOutput,
        mode: ThermostatMode,
        fan_mode: ThermostatFanMode,
        temperatureTarget: number,
        temperature: number,
        humidity: number
    }
}

export class ThermostatClient {

    private readonly http: AxiosInstance

    public constructor() {
        const url = `http://${window.location.hostname}:8080/`
        this.http = axios.create({
            // todo: configurable base url
            baseURL: url,
            timeout: 10000
        })
    }

    public async setFanMode(mode: ThermostatFanMode): Promise<void> {
        await this.command('set_fan_mode', {
            fan_mode: mode
        })
    }

    public async setMode(mode: ThermostatMode): Promise<void> {
        await this.command('set_mode', {
            mode
        })
    }

    public async setTemperatureTarget(target: number): Promise<void> {
        await this.command('set_temperature_target', {
            target
        })
    }

    public async report(): Promise<ThermostatReport> {
        return await this.command('report')
    }

    private async command<TRequest, TResponse>(name: string, request?: TRequest): Promise<any> {
        const response = await this.http.post('command', {
            cmd: name,
            ...request
        })
        return response.data as TResponse
    }

}