const mongoose = require('mongoose')

const DataSchema = mongoose.Schema({
    boa_id: [{type: mongoose.Schema.Types.ObjectId, ref: 'Boe'}],
    date: {type: Date, default: Date.now},
    wave: {height: Number, period: Number, power: Number}
})

module.exports = mongoose.model('WaveData', DataSchema)