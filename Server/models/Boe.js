const mongoose = require('mongoose')

const BoeSchema = mongoose.Schema({
    name: String,
    position: {lat: Number, lon: Number}
})


module.exports = mongoose.model('Boe', BoeSchema)
