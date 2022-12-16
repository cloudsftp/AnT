#include "CoexistingAttractorScan.hpp"

#include <iostream>

CAScanItemSequence::CAScanItemSequence (IterData* iterData)
  : ScanItemSequence (iterData),
    scanState (SCAN_FORWARD),
    hasScanItems (true)
{}

CAScanItemSequence::~CAScanItemSequence ()
{}

void CAScanItemSequence::operator>> (ostream& os) const
{
  // first output scan point..
  ScanItemSequence::operator>> (os);

  // ..then attractor id
  tracker->operator>> (os);
  os << " ";
}

void CAScanItemSequence::initialize (Configuration& scanDescription)
{
  Configuration casDescription
    = scanDescription.getSubConfiguration ("CAS_KEY");

  // call parent class to initialize scan items
  ScanItemSequence::initialize (scanDescription);

  seq_t::iterator i = sequence.begin ();

  if (i == sequence.end () || (*i) == NULL) {
    // no scan items - do just one iss
    hasScanItems = false;

    // 0:    no backtracking needed
    // true: add a progress writer to the iss scan
    tracker = new Tracker (casDescription, 0, true);

    return;
  }

  // save a pointer to the primary scan item
  primaryScanItem =
    dynamic_cast<IndexableScanItem*> (*i);

  if (primaryScanItem == NULL) {
    cerr << "Primary scan item must be indexable for coexisting attractor "
	 << "scan, from_file not supported."
	 << endl << Error::Exit;
  }

  issPeriod = casDescription.getLong ("STEPS_BETWEEN_ISS_KEY");

  // initialize attractor tracker
  tracker = new Tracker (casDescription,
			 primaryScanItem->getNumPoints () / issPeriod,
			 false);
}

bool CAScanItemSequence::startOfScanLineReached ()
{
  return (primaryScanItem->getCurrentIndex () <= 0);
}

bool CAScanItemSequence::endOfScanLineReached ()
{
  return (primaryScanItem->getCurrentIndex () >=
	  primaryScanItem->getNumPoints () - 1);
}

bool CAScanItemSequence::checkpointReached ()
{
  return (primaryScanItem->getCurrentIndex () ==
	  nextCheckpoint);
}

void CAScanItemSequence::saveIndex ()
{
  savedIndex = primaryScanItem->getCurrentIndex ();
}

void CAScanItemSequence::restoreIndex ()
{
  primaryScanItem->setCurrentIndex (savedIndex);
}

void CAScanItemSequence::scanNextInterval (ScanState state)
{
  scanState = state;

  switch (state) {
  case SCAN_FORWARD:
    nextCheckpoint =
      primaryScanItem->getCurrentIndex () +
      issPeriod;
    break;
  case SCAN_BACKWARD:
    nextCheckpoint =
      primaryScanItem->getCurrentIndex () -
      issPeriod;
    break;
  }
}

bool CAScanItemSequence::startForwardScanning ()
{
  if (endOfScanLineReached ()) {
    // reached the end, start next scan line
    nextScanLine ();

    return false;
  }

  // save start of forward scan
  saveIndex ();

  // add next checkpoint
  tracker->addCheckpoint ();
  
  // start scanning first attractor of current checkpoint
  tracker->firstAttractor (-1);
  
  scanNextInterval (SCAN_FORWARD);
  
  return true;
}

bool CAScanItemSequence::startBackwardScanning ()
{
  if (startOfScanLineReached ()) {
    // reached the start, don't backtrack
    return false;
  }

  // save start of backward scan
  saveIndex ();
 
  // start scanning first attractor of current checkpoint
  tracker->firstAttractor (0);
  
  // initialize depth counter
  backtrackDepth = 0;
  
  scanNextInterval (SCAN_BACKWARD);
  
  return true;
}

bool CAScanItemSequence::handleForwardCheckpoint ()
{
  tracker->singlePointScan (0);
  
  if (tracker->nextAttractor (-1)) {
    // still attractors in queue, rescan last interval
    restoreIndex ();
  } else {
    // no more attractors in queue, run iss scan
    tracker->runISS ();

    if (!startBackwardScanning ()) {
      // at first index, begin forward scanning instead

      return startForwardScanning ();
    }
  }
  
  return true;
}

bool CAScanItemSequence::handleBackwardCheckpoint ()
{
  // increase depth counter
  ++backtrackDepth;

  bool attractorKnown = !tracker->singlePointScan (-backtrackDepth);
 
  if (attractorKnown
      || backtrackDepth >= tracker->maxBacktrackDepth
      || startOfScanLineReached ()) {
    // stop backtracking if either of these apply:
    //  -current attractor is already known in reached snapshot
    //  -maximum backtracking depth reached
    //  -start of scan line reached
    if (!attractorKnown
	&& backtrackDepth >= tracker->maxBacktrackDepth) {
      cout << "WARNING: backtracking stopped because maximum depth was reached."
	   << endl;
    }

    // reset scan to start of backtracking session
    restoreIndex ();
    backtrackDepth = 0;

    if (!tracker->nextAttractor (0)) {
      // no attractors left to backtrack, start forward scanning
      return startForwardScanning ();
    }
  } else {
    scanNextInterval (SCAN_BACKWARD);

    dec ();
  }
  
  return true;
}

void CAScanItemSequence::standaloneScanNext ()
{
  if (firstCall) {
    // don't increment yet on the first call of this method
    firstCall = false;

    startScanLine ();

    return;
  }

  if (!hasScanItems) {
    finalFlag = !tracker->nextAttractor (0);

    return;
  }

  switch (scanState) {
  case SCAN_FORWARD:
    if (checkpointReached () || endOfScanLineReached ()) {
      handleForwardCheckpoint ();
    } else {
      inc ();
    }
    break;
  case SCAN_BACKWARD:
    if (checkpointReached () || startOfScanLineReached ()) {
      handleBackwardCheckpoint ();
    } else {
      dec ();
    }
    break;
  }

  set ();
}

bool CAScanItemSequence::dec ()
{
  // only decrement primary ScanItem
  return primaryScanItem->dec ();
}

void CAScanItemSequence::startScanLine ()
{
  // create new checkpoint and reset attractor IDs..
  tracker->reset ();

  // ..set system parameters..
  set ();

  // ..run a full ISS..
  tracker->runISS ();

  if (hasScanItems) {
    // ..and start scanning forward. (no backtracking from the first point
    // in the line!)
    startForwardScanning ();
  } else {
    // we don't have any scan items, just go through the attractors
    // found in the iss phase
    tracker->firstAttractor (0);
  }

}

void CAScanItemSequence::nextScanLine ()
{
  assert (endOfScanLineReached ());

  finalFlag = inc ();

  if (!finalFlag) startScanLine ();
}
