const express = require('express')
const router = express.Router()
const Boe = require('../models/Boe')
const Data = require('../models/Data')
const WaveData = require('../models/WaveData')
const User = require('../models/User')

router.get('/', (req, res) => {
    res.send('We are on API')
})

// All buoys with Lat e Lon
router.get('/buoys', async (req, res) => {
    try{
        const boe_list = await Boe.find()
        res.json(boe_list)
    }catch(err){
        res.send({message: err})
    }
})

// All single buoy data with name -> buoyName
router.get('/:buoyName', async (req, res) =>{
    try{
        const id = await Boe.find({name: req.params.buoyName})
        const data = await Data.find({boa_id: id[0]._id})

        res.json(data)
    }catch(err){
        res.send(err)
    }
})

/*router.delete('/:buoyName/delete',async (req,res) =>{
    try {
        const removed_boa = Boe.remove({name: req.params.buoyName})
        res.json(removed_boa)
    }catch(err){
        res.json({message: err})
    }
})*/

// Lat e Lon from buoy named -> buoyName
router.get('/:buoyName/position', async (req, res) =>{
    try{
        const data = await Boe.find({name: req.params.buoyName})
        res.json(data[0].position)
    }catch(err){
        res.send(err)
    }
})

// Get buoy id from name
async function getId(nameB){
    if(nameB == null){
        return {Error: "name boa is null"}
    }
    const boa = await Boe.find({name: nameB})
    const boaId = boa[0]._id

    return boaId

}

// Temperature from buoy named -> buoyName
router.get('/:buoyName/temperature', async (req, res) =>{
    try{
	    const boaId = await getId(req.params.buoyName)
        const data = await Data.find({boa_id: boaId}, 'date temperature')
        res.json(data)
    }catch(err){
        res.send(err)
    }
})

// Salinity from buoy named -> buoyName
router.get('/:buoyName/salinity', async (req, res) =>{
    try{
        const boaId = await getId(req.params.buoyName)
        const data = await Data.find({boa_id: boaId}, 'date salinity')
        res.json(data)
    }catch(err){
        res.send(err)
    }
})

// Wave data from buoy named -> buoyName
router.get('/:buoyName/wave', async (req, res) =>{
    try{
        const boaId = await getId(req.params.buoyName)
        const data = await WaveData.find({boa_id: boaId}, 'date wave')
        res.json(data)
    }catch(err){
        res.send(err)
    }
})

// Username data with name -> username
router.post('/:username', async (req, res) =>{
    try{
        const data = await User.find({name: req.params.username})
        res.json(data)
    }catch(err){
        res.send(err)
    }
})

// Update favorite buoy
router.get('/:username/change-favourite/:new', async (req, res) =>{
    try{
        const resUpdate = await User.updateOne({ name: req.params.username }, { favorite_buoy: req.params.new === "empty" ? "" : req.params.new });
        res.send(req.params.new)
    }catch(err){
        res.send(err)
    }
})


module.exports = router;
