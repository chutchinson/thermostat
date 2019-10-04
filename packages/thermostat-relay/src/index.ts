import * as express from 'express'
import * as cors from 'cors'
import { Thermostat } from './thermostat'

const ip = '192.168.86.99'
const app = express()
const thermostat = new Thermostat(ip)

type ThermostatOk = { cmd: string }
type ThermostatError = { ts: string, cmd: string, error: string }
type ThermostatResponse = ThermostatOk | ThermostatError

app.use(cors())
app.use(express.json())

app.get('/', (_, response) => {
    response.status(200).send('hello')
})

app.post('/command', async (request, response) => {
    const res = await thermostat.command(request.body)
    if (res.error) {
        response.status(500).send(res)
    }
    else {
        response.status(200).send(res)
    }
})

app.listen(8080, () => {
    console.log('[thermostat] relay listening')
})