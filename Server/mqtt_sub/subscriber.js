//Subscriber adds data coming from Arduino via MQTT

const mqtt = require('mqtt')
const mongoose = require('mongoose')
const Boe = require('./models/Boe')
const Data = require('./models/Data')
const WaveData = require('./models/WaveData')

//Database connection
const db = ""

mongoose.connect(db, {useUnifiedTopology: true, useNewUrlParser: true}).then(()=>{
    console.log('Connesso mongoose')
}).catch(err =>{
    console.log("Errore connessione Mongoose")
})

let client = mqtt.connect('mqtt://localhost:1883')
let topicData = '/floatter/data'
let topicWave = '/floatter/wave'


client.on('connect', () => {
    client.subscribe(topicData)
    client.subscribe(topicWave)
})


client.on('message', async (topic, message) => {
    message = message.toString()
    message = JSON.parse(message)

    //console.log(message)
    Boe.countDocuments({name: message.boa.name}, (err, count) => {
        if(count===0){
            Boe.insertMany([{
                name: message.boa.name,
                position: {lat: parseFloat(message.boa.lat), lon: parseFloat(message.boa.lon)}
            }])
        }
    })

    const boa = await Boe.find({name: message.boa.name})
    const boaId = boa[0]._id

    if(message.hasOwnProperty('wave')){
        WaveData.insertMany([{
            boa_id: boaId,
            wave: {height: message.wave.height, period: message.wave.period, power: message.wave.power}
        }])
    }else{
        Data.insertMany([{
            boa_id: boaId,
            temperature: message.temperature,
            salinity: message.salinity
        }])
    }
})