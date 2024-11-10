/*
 *  Simple Gate (header)
 *
 *  File        : SimpleGate.h
 *  Library     : SimpleSource
 *  Version     : 1.12
 *  Class       : SimpleGate, SimpleGateRms
 *
 *  2006, ChunkWare Music Software, OPEN-SOURCE
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */


#ifndef chunkware__SIMPLE_GATE_H
#define chunkware__SIMPLE_GATE_H

#include "SimpleHeader.h"       // common header
#include "SimpleEnvelope.h"     // for base class
#include "SimpleGain.h"         // for gain functions

namespace chunkware_simple
{
    //! simple gate
    class SimpleGate : public AttRelEnvelope
    {
    public:
        //! Constructor
        SimpleGate();

        //! Destructor
        virtual ~SimpleGate() {}

        //! set threshold
        virtual void   setThresh(double dB);

        //! get threshold
        virtual double getThresh(void) const { return threshdB_; }

        //! Init runtime
        //! \remark call before runtime (in resume())
        virtual void initRuntime(void);

        //! Process audio
        //! \remark gate runtime process
        void process(double &in1, double &in2);

        //! Process audio stereo-linked
        void process(double &in1, double &in2, double keyLinked);    // with stereo-linked key in

    private:
        // transfer function
        double threshdB_;   //!< threshold (dB)
        double thresh_;     //!< threshold (linear)

        // runtime variables
        double env_;        //!< over-threshold envelope (linear)
    };

    //! Simple gate with RMS detection
    class SimpleGateRms : public SimpleGate
    {
    public:
        //! Constructor
        SimpleGateRms();

        //! Destructor
        virtual ~SimpleGateRms() override {}

        //! set sample rate
        virtual void setSampleRate(double sampleRate) override;

        //! set RMS window
        virtual void setWindow(double ms);

        //! get RMS window
        virtual double getWindow(void) const { return ave_.getTc(); }

        //! call before runtime (in resume())
        virtual void initRuntime(void) override;

        //! gate runtime process
        void process(double &in1, double &in2);

    private:
        EnvelopeDetector ave_;  //!< averager
        double aveOfSqrs_;      //!< average of squares

    };  // end SimpleGateRms class

}   // end namespace chunkware_simple

// include inlined process function
#include "SimpleGateProcess.inl"

#endif  // end __SIMPLE_GATE_H__
