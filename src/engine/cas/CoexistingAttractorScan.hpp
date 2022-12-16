#ifndef CONCURRENTATTRACTORSCAN_HPP
#define CONCURRENTATTRACTORSCAN_HPP

#include "AttractorTracking.hpp"

/**
 * This class implements the coexisting attractor scan
 * (nested_items_cas)
 */
class CAScanItemSequence : public ScanItemSequence {
private:
  enum ScanState {
    SCAN_FORWARD,      // regular scanning
    SCAN_BACKWARD,     // backtracking
  };

  Tracker* tracker;

  ScanState scanState;

  IndexableScanItem* primaryScanItem;

  bool hasScanItems;

  integer_t issPeriod;
  integer_t backtrackDepth;
  integer_t nextCheckpoint;

  /**
   * Checks if we are at the start of a scan line
   */
  bool startOfScanLineReached ();

  /**
   * Checks if we are at the end of a scan line
   */
  bool endOfScanLineReached ();

  /**
   * Checks if we are at an initial state scan checkpoint
   */
  bool checkpointReached ();

  integer_t savedIndex;

  /**
   * Saves the current position of the primary scan item
   */
  void saveIndex ();

  /**
   * Restores the saved position of the primary scan item
   * @see CAScanItemSequence::saveIndex
   */
  void restoreIndex ();

  /**
   * Start a scan up to the next checkpoint in the given direction
   */
  void scanNextInterval (ScanState state);

  /**
   * Start scanning all found attractors
   * forward from the current position
   */
  bool startForwardScanning ();

  /**
   * Start scanning all found attractors
   * backward from the current position
   */
  bool startBackwardScanning ();

  /**
   * Handle checkpoint while scanning forward
   */
  bool handleForwardCheckpoint ();

  /**
   * Handle checkpoint while scanning backward
   */
  bool handleBackwardCheckpoint ();

protected:
  /**
   * Decrease primary scan item
   */
  bool dec ();

  /**
   * Start a scan line
   */
  void startScanLine ();

  /**
   * Start a new scan line
   */
  void nextScanLine ();

public:
  /**
   * Sole constructor of this class
   */
  CAScanItemSequence (IterData* iterData);
  virtual ~CAScanItemSequence ();

  /**
   * Initializes the scan items and the tracker
   */
  void initialize (Configuration& scanDescription);

  /**
   * Output current scan point
   */
  void operator>> (ostream&) const;

  /**
   * Called by the main simulator to generate the next scan point
   * in a standalone scan.
   */
  void standaloneScanNext ();

  /**
   * Called by the main simulator to get the next scan point from
   * the server in a distributed scan. Not implemented here
   */
  void netClientScanNext () {
    assert (false);
  };
  /**
   * Called by the main simulator to generate the next scan points
   * for the clients in a distributed scan. Not implemented here
   */
  void netServerScanNext () {
    assert (false);
  };
};

#endif
