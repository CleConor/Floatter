#!/usr/bin/env node
const express = require('express')
const app = express()
const mongoose = require('mongoose')
var bodyParser = require('body-parser')

app.use(bodyParser.text())

//Importing Routes

const apiR = require('./routes/api');
app.use('/api', apiR);

app.get('/', (req, res) => {
	res.send('Floatter.it')
})


//Database connection
const db = ""

mongoose.connect(db, {useUnifiedTopology: true, useNewUrlParser: true}).then(()=>{
	console.log('Mongoose connected')
}).catch(err =>{
	console.log("Mongoose Error")
})

app.listen(8080)
