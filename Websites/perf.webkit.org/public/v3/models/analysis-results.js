
class AnalysisResults {
    constructor()
    {
        this._metricToBuildMap = {};
        this._metricIds = [];
        this._lazilyComputedHighestTests = new LazilyEvaluatedFunction(this._computeHighestTests);
    }

    findResult(buildId, metricId)
    {
        const map = this._metricToBuildMap[metricId];
        if (!map)
            return null;
        return map[buildId];
    }

    highestTests() { return this._lazilyComputedHighestTests.evaluate(this._metricIds); }

    containsTest(test)
    {
        console.assert(test instanceof Test);
        for (const metric of test.metrics()) {
            if (metric.id() in this._metricToBuildMap)
                return true;
        }
        return false;
    }

    _computeHighestTests(metricIds)
    {
        const testsInResults = new Set(metricIds.map((metricId) => Metric.findById(metricId).test()));
        return [...testsInResults].filter((test) => !testsInResults.has(test.parentTest()));
    }

    add(measurement)
    {
        console.assert(measurement.configType == 'current');
        const metricId = measurement.metricId;
        if (!(metricId in this._metricToBuildMap)) {
            this._metricToBuildMap[metricId] = {};
            this._metricIds = Object.keys(this._metricToBuildMap);
        }
        const map = this._metricToBuildMap[metricId];
        console.assert(!map[measurement.buildId]);
        map[measurement.buildId] = measurement;
    }

    commitSetForRequest(buildRequest)
    {
        if (!this._metricIds.length)
            return null;
        const result = this.findResult(buildRequest.buildId(), this._metricIds[0]);
        if (!result)
            return null;
        return result.commitSet();
    }

    viewForMetric(metric)
    {
        console.assert(metric instanceof Metric);
        return new AnalysisResultsView(this, metric);
    }

    static fetch(taskId)
    {
        taskId = parseInt(taskId);
        return RemoteAPI.getJSONWithStatus(`../api/measurement-set?analysisTask=${taskId}`).then(function (response) {

            Instrumentation.startMeasuringTime('AnalysisResults', 'fetch');

            const adaptor = new MeasurementAdaptor(response['formatMap']);
            const results = new AnalysisResults;
            for (const rawMeasurement of response['measurements'])
                results.add(adaptor.applyToAnalysisResults(rawMeasurement));

            Instrumentation.endMeasuringTime('AnalysisResults', 'fetch');

            return results;
        });
    }
}

class AnalysisResultsView {
    constructor(analysisResults, metric)
    {
        console.assert(analysisResults instanceof AnalysisResults);
        console.assert(metric instanceof Metric);
        this._results = analysisResults;
        this._metric = metric;
    }

    metric() { return this._metric; }

    resultForRequest(buildRequest)
    {
        return this._results.findResult(buildRequest.buildId(), this._metric.id());
    }
}
