/*
 *  Simple Compressor (header)
 *
 *  File        : SimpleComp.h
 *  Library     : SimpleSource
 *  Version     : 1.12
 *  Class       : SimpleComp, SimpleCompRms
 *
 *  © 2006, ChunkWare Music Software, OPEN-SOURCE
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

#ifndef chunkware_simple_SIMPLE_COMP_H
#define chunkware_simple_SIMPLE_COMP_H

#include "SimpleHeader.h"       // common header
#include "SimpleEnvelope.h"     // for base class
#include "SimpleGain.h"         // for gain functions

namespace chunkware_simple
{
    //! simple compressor
    class SimpleComp : public AttRelEnvelope
    {
    public:
        //! Ctor
        SimpleComp();

        //! Dtor
        virtual ~SimpleComp() {}

        //! set parameters
        //! @{
        virtual void setThresh(double dB);
        virtual void setRatio(double dB);
        void setMakeUpGain(double gain);
        //! @}

        //! get parameters
        //! @{
        virtual double getThresh(void) const { return threshdB_; }
        virtual double getRatio(void)  const { return ratio_; }
        double getMakeUpGain(void) const     { return makeUpGain_; }
        //! @}

        //! Init runtime
        //! \pre Call before runtime (in resume())
        virtual void initRuntime(void);

        //! compressor runtime process
        void process(double &in1, double &in2);

        //! process sample with stereo-linked key in
        void process(double &in1, double &in2, double keyLinked);

    private:
        // transfer function
        double threshdB_;       // threshold (dB)
        double ratio_;          // ratio (compression: < 1 ; expansion: > 1)

        // runtime variables
        double envdB_;          // over-threshold envelope (dB)
        double makeUpGain_ = 1.0;

    };  // end SimpleComp class

    //! Simple compressor with RMS detection
    class SimpleCompRms : public SimpleComp
    {
    public:
        //! Ctor
        SimpleCompRms();

        //! Dtor
        virtual ~SimpleCompRms() override {}

        //! Sample rate
        virtual void setSampleRate(double sampleRate) override;

        //! RMS window
        //! @{
        virtual void setWindow(double ms);
        virtual double getWindow(void) const  { return ave_.getTc(); }
        //! @}

        //! Runtime process
        //! @{
        virtual void initRuntime(void) override; // call before runtime (in resume())
        void process(double &in1, double &in2);  // compressor runtime process
        //! @}

    private:

        EnvelopeDetector ave_;  // averager
        double aveOfSqrs_;      // average of squares

    };  // end SimpleCompRms class

}   // end namespace chunkware_simple

// include inlined process function
#include "SimpleCompProcess.inl"

#endif  // guard
