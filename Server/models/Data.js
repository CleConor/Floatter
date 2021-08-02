const mongoose = require('mongoose')

const DataSchema = mongoose.Schema({
    boa_id: [{type: mongoose.Schema.Types.ObjectId, ref: 'Boe'}],
    date: {type: Date, default: Date.now},
    temperature: Number,
    salinity: Number,
    //accelerometer: {x: Number, y: Number, z: Number}
})

module.exports = mongoose.model('Data', DataSchema)
