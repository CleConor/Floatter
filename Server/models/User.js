const mongoose = require('mongoose')

const DataSchema = mongoose.Schema({
    name: String,
    password: String,
    salt: String,
    favorite_buoy: String
})

module.exports = mongoose.model('User', DataSchema)