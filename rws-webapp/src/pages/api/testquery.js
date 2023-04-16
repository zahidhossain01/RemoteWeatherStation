// GET /api/testquery?id=mom&name=jason

export default function handler(req, res) {
    res.status(200).json({ query: req.query })
}
