const express = require('express');
const router = express.Router();
const Joi = require('joi');
// const path = require('path');

const jif_db = require('../jifdb');
let sessions = jif_db.open_collection({collection_name: "sessions"});

// router.get('/', function(req, res, next) {
router.get('/', async (req, res) => {
  try {
    // let this_query = req.query;
    // let query_last = this_query["last"] || "10";
    // let this_last = parseInt(query_last, 10) || 10;
    const get_sessions = sessions.read({});
    res.send(get_sessions);
  } catch(err) {
    console.log("error " + err);
  }
});

// https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
// 2xx success
// 200 OK
//   Standard response for successful HTTP requests. The actual response will depend on the request method used. In a GET request, the response will contain an entity corresponding to the requested resource. In a POST request, the response will contain an entity describing or containing the result of the action.[8]
// 3xx redirection
// 304 Not Modified (RFC 7232)
// 4xx client errors
// 400 Bad Request
// 401 Unauthorized (RFC 7235)
// 402 Payment Required
// 403 Forbidden
// 404 Not anchorNrFound
// 405 Method Not Allowed
// 5xx server errors
// 500 Internal Server Error
// 501 Not Implemented
// 502 Bad Gateway
// 503 Service Unavailable
// 504 Gateway Timeout

router.get('/:id', async (req, res) => {
  const req_params_id = req.params.id || 0;
  const get_session = sessions.read({id:req_params_id});
  if (!get_session) return res.status(404).send('The session with the given ID was not found.');
  res.send(get_session);
});

router.post('/', async (req, res) => {
  // let remoteAddress = req.headers['x-forwarded-for'] || req.socket.remoteAddress 
  // console.log(`# (post) req.body="${ JSON.stringify(req.body) }" `);
  const { error } = validatesession(req.body); 
  if (error) return res.status(400).send(error.details[0].message);
  try {
    let new_session_hash = { 
      firstname: req.body.firstname,
      lastname: req.body.lastname,
    };
    let new_session = sessions.create({item:new_session_hash});
    res.send(new_session);
  } catch(err) {
    console.log("error " + err);
  }
});

router.put('/:id', async (req, res) => {
  const { error } = validatesession(req.body); 
  if (error) return res.status(400).send(error.details[0].message);
  const edit_session = {placeholder:true};
  if (!edit_session) return res.status(404).send('The session with the given ID was not found.');
  res.send(edit_session);
});

router.delete('/:id', async (req, res) => {
  // const del_session = await session.findByIdAndRemove(req.params.id);
  const del_session = {placeholder:true};
  if (!del_session) return res.status(404).send('The session with the given ID was not found.');
  res.send(del_session);
});

// https://joi.dev/api/?v=17.4.2
function validatesession(session) {
  const schema = Joi.object({
    name: Joi.string().required(),
    // lastname: Joi.string().required(),
  });  
  const validation = schema.validate(session);
  return validation;
}

module.exports = router;

//-EOF