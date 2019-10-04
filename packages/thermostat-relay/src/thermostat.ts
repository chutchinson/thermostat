import * as dgram from 'dgram'

export class Thermostat {
 
    private socket: dgram.Socket
    private address: string
    private port: number

    public constructor(address: string = '127.0.0.1', port: number = 8888) {
        this.socket = dgram.createSocket('udp4')
        this.address = address
        this.port = port
    }

    public command<TRequest>(request: TRequest): Promise<any> {
        return new Promise((resolve, reject) => {
            const message = JSON.stringify(request)
            this.socket.send(message, this.port, this.address, (err) => {
                if (err) {
                    reject()
                    return
                }
            })
            const error = (err: Error) => {
                this.socket.removeListener('error', error)
                reject(err)
            }
            const receive = (buffer: Buffer) => {
                this.socket.removeListener('error', error)
                this.socket.removeListener('message', receive)
                const data = JSON.parse(buffer.toString())
                resolve(data)
            }
            this.socket.once('error', error)
            this.socket.once('message', receive)
        })
    }

}