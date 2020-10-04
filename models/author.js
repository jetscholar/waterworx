const mongoose = require('mongoose')

const authorSchema = new mongoose.Schema({
    name: {
        type: String,
        required: true
    }
})

// name the model - name of table in db - using German
module.exports = mongoose.model('Author', authorSchema)