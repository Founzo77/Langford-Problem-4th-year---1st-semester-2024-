import matplotlib.pyplot as plt
import json
import numpy as np
import os
from jinja2 import Environment, FileSystemLoader

def calculate_stats(stat):
    if len(stat) == 0:
        return {
            'mean': 0,
            'std': 0,
            'min': 0,
            'max': 0,
            'count': 0
        }
    return {
        'mean': np.mean(stat),
        'std': np.std(stat),
        'min': np.min(stat),
        'max': np.max(stat),
        'count': len(stat)
    }

class Dispatcher:
    id = 0
    events = []
    workersId = []
    baseDirectoryOutputPath = ""

    def __init__(self, jsonSrcFile, baseDirectoryOutputPath):
        self.baseDirectoryOutputPath = baseDirectoryOutputPath
        with open(jsonSrcFile, 'r', encoding='utf-8') as file:
            jsonData = json.load(file)
            self.events = jsonData["events"]
            self.id = jsonData["id"]
            self.workersId = jsonData["workersId"]

    def generateWaitGetAddByTimeGraph(self):
        durationsWait = []
        timeWait = []
        durationsAdd = []
        timeAdd = []
        durationsGet = []
        timeGet = []

        waitCumul = [0]
        addCumul = [0]
        getCumul = [0]
        timeCumul = [0]

        for event in self.events:
            timeCumul.append(event['duration'] + timeCumul[-1])
            if(event['type'] == 'tasks_requested'):
                durationsGet.append(event['duration'])
                timeGet.append(timeCumul[-1])
                getCumul.append(getCumul[-1] + event['duration'])
            else:
                getCumul.append(getCumul[-1])

            if(event['type'] == 'tasks_added'):
                durationsAdd.append(event['duration'])
                timeAdd.append(timeCumul[-1])
                addCumul.append(addCumul[-1] + event['duration'])
            else:
                addCumul.append(addCumul[-1])

            if(event['type'] == 'wait'):
                durationsWait.append(event['duration'])
                timeWait.append(timeCumul[-1])
                waitCumul.append(waitCumul[-1] + event['duration'])
            else:
                waitCumul.append(waitCumul[-1])

        waitCumul.pop(0)
        addCumul.pop(0)
        getCumul.pop(0)
        timeCumul.pop(0)

        plt.figure(figsize=(12, 6))
        plt.plot(timeCumul, waitCumul, marker='o', linestyle='-', label='Attente')
        plt.plot(timeCumul, addCumul, marker='o', linestyle='-', label='Add')
        plt.plot(timeCumul, getCumul, marker='o', linestyle='-', label='Get')

        plt.title('Durée cumulée d\'actions en fonctions du temps')
        plt.xlabel('Temps (ms)')
        plt.ylabel('Durée cumulée (ms)')
        plt.legend()
        plt.grid(True)
        plt.savefig(f"{self.baseDirectoryOutputPath}/dispatcher_{self.id}_cumul_wait_add_get.png")
        plt.close()

        plt.figure(figsize=(12, 6))
        plt.plot(timeAdd, durationsAdd, marker='o', linestyle='-', label='Add')
        plt.plot(timeGet, durationsGet, marker='o', linestyle='-', label='Get')
        plt.plot(timeWait, durationsWait, marker='o', linestyle='-', label='Attente')

        plt.title('Durée d\'actions en fonctions du temps')
        plt.xlabel('Temps (ms)')
        plt.ylabel('Durée (ms)')
        plt.legend()
        plt.grid(True)
        plt.savefig(f"{self.baseDirectoryOutputPath}/dispatcher_{self.id}_wait_add_get.png")
        plt.close()

    def generateNbTasksOverTimeGraph(self):
        timeCumul = [0]
        nbTaskCumul = [0]

        for event in self.events:
            if event['type'] == 'tasks_added':
                timeCumul.append(timeCumul[-1] + event['duration'])
                nbTaskCumul.append(nbTaskCumul[-1] + event['numberOutput'])
            elif event['type'] == 'tasks_requested':
                timeCumul.append(timeCumul[-1] + event['duration'])
                nbTaskCumul.append(nbTaskCumul[-1] - event['numberOutput'])
            elif event['type'] == 'wait':
                timeCumul.append(timeCumul[-1] + event['duration'])
                nbTaskCumul.append(nbTaskCumul[-1])
        
        timeCumul.pop(0)
        nbTaskCumul.pop(0)

        plt.figure(figsize=(12, 6))
        plt.plot(timeCumul, nbTaskCumul, marker='o', linestyle='-', label='nb Tasks')

        plt.title('Tâches cumulées en fonctions du temps')
        plt.xlabel('Temps (ms)')
        plt.ylabel('Nombre de tâches')
        plt.legend()
        plt.grid(True)
        plt.savefig(f"{self.baseDirectoryOutputPath}/dispatcher_{self.id}_cumul_nb_tasks.png")
        plt.close()

    def generateNbTasksOverTimePerWorkerGraph(self):
        timeCumul = [0]
        nbTaskGetCumulPerWorker = {}
        nbTaskAddCumulPerWorker = {}

        for workerId in self.workersId:
            nbTaskGetCumulPerWorker[workerId] = [0]
            nbTaskAddCumulPerWorker[workerId] = [0]

        for event in self.events:
            timeCumul.append(timeCumul[-1] + event['duration'])
            for workerId in self.workersId:
                if event['type'] != 'wait' and event["workerId"] == workerId:
                    if event['type'] == 'tasks_added':
                        nbTaskAddCumulPerWorker[workerId].append(event['numberOutput'] + nbTaskAddCumulPerWorker[workerId][-1])
                        nbTaskGetCumulPerWorker[workerId].append(nbTaskGetCumulPerWorker[workerId][-1])
                    elif event['type'] == 'tasks_requested':
                        nbTaskGetCumulPerWorker[workerId].append(event['numberOutput'] + nbTaskGetCumulPerWorker[workerId][-1])
                        nbTaskAddCumulPerWorker[workerId].append(nbTaskAddCumulPerWorker[workerId][-1])
                else:
                    nbTaskGetCumulPerWorker[workerId].append(nbTaskGetCumulPerWorker[workerId][-1])
                    nbTaskAddCumulPerWorker[workerId].append(nbTaskAddCumulPerWorker[workerId][-1])

        timeCumul.pop(0)

        for workerId in self.workersId:
            nbTaskAddCumulPerWorker[workerId].pop(0)
            nbTaskGetCumulPerWorker[workerId].pop(0)

            plt.figure(figsize=(12, 6))
            plt.plot(timeCumul, nbTaskAddCumulPerWorker[workerId], marker='o', linestyle='-', label='nb Tasks Add')
            plt.plot(timeCumul, nbTaskGetCumulPerWorker[workerId], marker='o', linestyle='-', label='nb Tasks Get')

            plt.title('Tâches cumulées en fonctions du temps')
            plt.xlabel('Temps (ms)')
            plt.ylabel('Nombre de tâches')
            plt.legend()
            plt.grid(True)
            plt.savefig(f"{self.baseDirectoryOutputPath}/dispatcher_{self.id}_worker_{workerId}_cumul_nb_tasks.png")
            plt.close()

    def getStats(self):
        durationsWait = []
        durationsGet = []
        durationsAdd = []

        for event in self.events:
            if event['type'] == 'tasks_requested':
                durationsGet.append(event['duration'])
            elif event['type'] == 'tasks_added':
                durationsAdd.append(event['duration'])
            elif event['type'] == 'wait':
                durationsWait.append(event['duration'])

        statsWait = calculate_stats(durationsWait)
        statsGet = calculate_stats(durationsGet)
        statsAdd = calculate_stats(durationsAdd)

        stats = {
            'wait': statsWait,
            'add': statsAdd,
            'get': statsGet
        }

        return stats

    def getDico(self):
        result = {
            'id': f"{self.id}",
            'stats': self.getStats(),
            'src_cumul_nb_tasks_per_worker': []
        }

        if os.path.exists(f"{self.baseDirectoryOutputPath}/dispatcher_{self.id}_cumul_wait_add_get.png"):
            result['src_cumul_attente_travail_dispatche'] = f"dispatcher_{self.id}_cumul_wait_add_get.png"
        if os.path.exists(f"{self.baseDirectoryOutputPath}/dispatcher_{self.id}_wait_add_get.png"):
            result['src_attente_travail_dispatche'] = f"dispatcher_{self.id}_wait_add_get.png"
        if os.path.exists(f"{self.baseDirectoryOutputPath}/dispatcher_{self.id}_cumul_nb_tasks.png"):
            result['src_cumul_nb_tasks'] = f"dispatcher_{self.id}_cumul_nb_tasks.png"

        for workerId in self.workersId:
            if os.path.exists(f"{self.baseDirectoryOutputPath}/dispatcher_{self.id}_worker_{workerId}_cumul_nb_tasks.png"):
                result['src_cumul_nb_tasks_per_worker'].append({
                    'src': f"dispatcher_{self.id}_worker_{workerId}_cumul_nb_tasks.png",
                    'workerId': workerId
                })

        return result

class Worker:
    events = []
    id = 0
    workDuration = 0
    baseDirectoryOutputPath = ""

    def __init__(self, jsonSrcFile, baseDirectoryOutputPath):
        self.baseDirectoryOutputPath = baseDirectoryOutputPath
        with open(jsonSrcFile, 'r', encoding='utf-8') as file:
            jsonData = json.load(file)
            self.events = jsonData["events"]
            self.workDuration = jsonData["workDuration"]
            self.id = jsonData["id"]

    def generateWaitWorkDispatchByTimeGraph(self):
        durationsWait = []
        timeWait = []
        durationsTask = []
        timeTask = []
        durationsDispatch = []
        timeDispatch = []

        waitCumul = [0]
        taskCumul = [0]
        dispatchCumul = [0]
        timeCumul = [0]

        for event in self.events:
            timeCumul.append(event['duration'] + timeCumul[-1])
            if(event['type'] == 'task_resolved'):
                durationsTask.append(event['duration'])
                timeTask.append(timeCumul[-1])
                taskCumul.append(taskCumul[-1] + event['duration'])
            else:
                taskCumul.append(taskCumul[-1])

            if(event['type'] == 'tasks_requested' or event['type'] == 'tasks_added'):
                durationsDispatch.append(event['duration'])
                timeDispatch.append(timeCumul[-1])
                dispatchCumul.append(dispatchCumul[-1] + event['duration'])
            else:
                dispatchCumul.append(dispatchCumul[-1])

            if(event['type'] == 'wait'):
                durationsWait.append(event['duration'])
                timeWait.append(timeCumul[-1])
                waitCumul.append(waitCumul[-1] + event['duration'])
            else:
                waitCumul.append(waitCumul[-1])

        waitCumul.pop(0)
        taskCumul.pop(0)
        dispatchCumul.pop(0)
        timeCumul.pop(0)

        plt.figure(figsize=(12, 6))
        plt.plot(timeCumul, waitCumul, marker='o', linestyle='-', label='Attente')
        plt.plot(timeCumul, dispatchCumul, marker='o', linestyle='-', label='Dispatch')
        plt.plot(timeCumul, taskCumul, marker='o', linestyle='-', label='Travail')

        plt.title('Durée cumulée d\'actions en fonctions du temps')
        plt.xlabel('Temps (ms)')
        plt.ylabel('Durée cumulée (ms)')
        plt.legend()
        plt.grid(True)
        plt.savefig(f"{self.baseDirectoryOutputPath}/worker_{self.id}_cumul_wait_work_dispatch.png")
        plt.close()

        plt.figure(figsize=(12, 6))
        plt.plot(timeTask, durationsTask, marker='o', linestyle='-', label='Travail')
        plt.plot(timeDispatch, durationsDispatch, marker='o', linestyle='-', label='Dispatch')
        plt.plot(timeWait, durationsWait, marker='o', linestyle='-', label='Attente')

        plt.title('Durée d\'actions en fonctions du temps')
        plt.xlabel('Temps (ms)')
        plt.ylabel('Durée (ms)')
        plt.legend()
        plt.grid(True)
        plt.savefig(f"{self.baseDirectoryOutputPath}/worker_{self.id}_wait_work_dispatch.png")
        plt.close()

    def getStats(self):
        durationsWait = []
        durationsTask = []
        durationsDispatch = []

        for event in self.events:
            if event['type'] == 'task_resolved':
                durationsTask.append(event['duration'])
            elif event['type'] == 'tasks_requested' or event['type'] == 'tasks_added':
                durationsDispatch.append(event['duration'])
            elif event['type'] == 'wait':
                durationsWait.append(event['duration'])

        statsWait = calculate_stats(durationsWait)
        statsTask = calculate_stats(durationsTask)
        statsDispatch = calculate_stats(durationsDispatch)

        stats = {
            'wait': statsWait,
            'task': statsTask,
            'dispatch': statsDispatch
        }

        return stats

    def getDico(self):
        result = {
            'id': f"{self.id}",
            'stats': self.getStats()
        }

        if os.path.exists(f"{self.baseDirectoryOutputPath}/worker_{self.id}_cumul_wait_work_dispatch.png"):
            result['src_cumul_attente_travail_dispatche'] = f"worker_{self.id}_cumul_wait_work_dispatch.png"
        if os.path.exists(f"{self.baseDirectoryOutputPath}/worker_{self.id}_wait_work_dispatch.png"):
            result['src_attente_travail_dispatche'] = f"worker_{self.id}_wait_work_dispatch.png"

        return result

class Resolver:
    count = 0
    frequency = 0
    initializationDuration = 0
    workDuration = 0
    mergeDuration = 0
    nbWorkers = 0
    nbDispatchers = 0
    metaName = ""

    def __init__(self, jsonSrcFile):
        print(jsonSrcFile)
        with open(jsonSrcFile, 'r', encoding='utf-8') as file:
            jsonData = json.load(file)
            self.count = jsonData["C"]
            self.frequency = jsonData["F"]
            self.initializationDuration = jsonData["initializationDuration"]
            self.workDuration = jsonData["workDuration"]
            self.mergeDuration = jsonData["mergeDuration"]
            self.nbWorkers = jsonData["nbWorkers"]
            self.nbDispatchers = jsonData["nbDispatchers"]
            self.metaName = jsonData["metaName"]        
        
    def getDico(self):
        return {
            'initializationDuration' : self.initializationDuration,
            'workDuration': self.workDuration,
            'mergeDuration': self.mergeDuration,
            'nbWorkers': self.nbWorkers,
            'nbDispatchers': self.nbDispatchers,
            'count': self.count,
            'frequency': self.frequency
        }

class Data:
    resolver = None
    dispatchers = []
    workers = []
    baseDirectoryOutputPath = ""

    def __init__(self, srcDirectory, baseDirectoryOutputPath):
        self.baseDirectoryOutputPath = baseDirectoryOutputPath
        self.resolver = Resolver(f"{srcDirectory}/resolver.json")

        for i in range(0, self.resolver.nbDispatchers):
            if os.path.exists(f"{srcDirectory}/dispatcher_{i}.json"):
                self.dispatchers.append(Dispatcher(f"{srcDirectory}/dispatcher_{i}.json", baseDirectoryOutputPath))

        for i in range(0, self.resolver.nbWorkers):
            if os.path.exists(f"{srcDirectory}/worker_{i}.json"):
                self.workers.append(Worker(f"{srcDirectory}/worker_{i}.json", baseDirectoryOutputPath))

    def getStats(self):
        combinedDurationsWaitWorkers = []
        combinedDurationsWaitDispatchers = []

        combinedDurationsTask = []
        combinedDurationsDispatch = []
        combinedDurationsAdd = []
        combinedDurationsGet = []

        for worker in self.workers:
            stats = worker.getStats()
            combinedDurationsWaitWorkers.extend([event['duration'] for event in worker.events if event['type'] == 'wait'])
            combinedDurationsTask.extend([event['duration'] for event in worker.events if event['type'] == 'task_resolved'])
            combinedDurationsDispatch.extend([event['duration'] for event in worker.events if event['type'] in ('tasks_requested', 'tasks_added')])

        for dispatcher in self.dispatchers:
            stats = dispatcher.getStats()
            combinedDurationsWaitDispatchers.extend([event['duration'] for event in dispatcher.events if event['type'] == 'wait'])
            combinedDurationsAdd.extend([event['duration'] for event in dispatcher.events if event['type'] == 'tasks_added'])
            combinedDurationsGet.extend([event['duration'] for event in dispatcher.events if event['type'] == 'tasks_requested'])

        statsWaitDispatchers = calculate_stats(combinedDurationsWaitDispatchers)
        statsWaitWorkers = calculate_stats(combinedDurationsWaitWorkers)
        statsTask = calculate_stats(combinedDurationsTask)
        statsDispatch = calculate_stats(combinedDurationsDispatch)
        statsAdd = calculate_stats(combinedDurationsAdd)
        statsGet = calculate_stats(combinedDurationsGet)

        stats = {
            'workersStats': {
                'wait': statsWaitWorkers,
                'dispatch': statsDispatch,
                'task': statsTask
            },
            'dispatchersStats' : {
                'wait': statsWaitDispatchers,
                'Add': statsAdd,
                'get': statsGet
            } 
        }

        return stats


    def generateGraph(self):
        for worker in self.workers:
            worker.generateWaitWorkDispatchByTimeGraph()
        for dispatcher in self.dispatchers:
            dispatcher.generateWaitGetAddByTimeGraph()
            dispatcher.generateNbTasksOverTimeGraph()
            dispatcher.generateNbTasksOverTimePerWorkerGraph()

    def generateHtml(self):
        env = Environment(loader=FileSystemLoader("template"))
        template = env.get_template('index.jinja2.html')

        data = {
            'resolver': self.resolver.getDico(),
            'dispatchers': [],
            'workers': [],
            'globalStats': self.getStats()
        }

        outWorkers = []
        for worker in self.workers:
            outWorkers.append(worker.getDico())
        data['workers'] = outWorkers

        outDispatchers = []
        for dispatcher in self.dispatchers:
            outDispatchers.append(dispatcher.getDico())
        data['dispatchers'] = outDispatchers

        output_from_parsed_template = template.render(data)

        output_file_path = os.path.join(self.baseDirectoryOutputPath, 'index.html')
        with open(output_file_path, 'w', encoding='utf-8') as f:
            f.write(output_from_parsed_template)

        print(f"Le fichier HTML a été généré avec succès à : {output_file_path}")

class CompareData:
    data = []

    def __init__(self, lSrcDirectory, baseDirectoryOutputPath):
        for srcDirectory in lSrcDirectory:
            self.data.append(Data(srcDirectory, baseDirectoryOutputPath))

    def generateTimeExecutionOverWorker(self, baseDirectoryOutputPath):
        workers = []
        timeExecution = []
        acceleration = []

        for data in self.data:
            workers.append(data.resolver.nbWorkers)
            timeExecution.append(data.resolver.initializationDuration + data.resolver.workDuration + data.resolver.mergeDuration)

        initialTime = timeExecution[0]

        for time in timeExecution:
            acceleration.append(initialTime / time)

        plt.figure(figsize=(12, 6))
        plt.plot(workers, timeExecution, marker='o', linestyle='-', label='Execution')
        plt.title('Temps d\'exécution en fonction du nombre de workers')
        plt.xlabel('Nombre de workers')
        plt.ylabel('Temps (ms)')
        plt.legend()
        plt.grid(True)
        plt.savefig(f"{baseDirectoryOutputPath}/compare_execution_over_worker.png")
        plt.close()

        plt.figure(figsize=(12, 6))
        plt.plot(workers, acceleration, marker='o', linestyle='-', label='Accélération')
        plt.title('Accélération en fonction du nombre de workers')
        plt.xlabel('Nombre de workers')
        plt.ylabel('Accélération (facteur)')
        plt.legend()
        plt.grid(True)
        plt.savefig(f"{baseDirectoryOutputPath}/compare_acceleration_over_worker.png")
        plt.close()

    def getStats(self):
        workers = []
        timeExecution = []
        acceleration = []

        for data in self.data:
            workers.append(data.resolver.nbWorkers)
            timeExecution.append(data.resolver.initializationDuration + data.resolver.workDuration + data.resolver.mergeDuration)

        initialTime = timeExecution[0]

        for time in timeExecution:
            acceleration.append(initialTime / time)

        elements = []

        for id, time in enumerate(timeExecution):
            elements.append({
                'metaName': self.data[id].resolver.metaName,
                'timeExecution' : timeExecution[id],
                'acceleration': acceleration[id],
                'nbWorkers': workers[id]
            })

        return {
            'elements': elements
        } 

    def getDico(self):
        return {
            'src_execution_over_worker': 'compare_execution_over_worker.png',
            'src_acceleration_over_worker': 'compare_acceleration_over_worker.png',
            'stats': self.getStats()
        }

    def generateGraph(self, baseDirectoryOutputPath):
        self.generateTimeExecutionOverWorker(baseDirectoryOutputPath)

    def generateHtml(self, baseDirectoryOutputPath):
        env = Environment(loader=FileSystemLoader("template"))
        template = env.get_template('index_compare.jinja2.html')

        data = self.getDico()

        output_from_parsed_template = template.render(data)

        output_file_path = os.path.join(baseDirectoryOutputPath, 'index.html')
        with open(output_file_path, 'w', encoding='utf-8') as f:
            f.write(output_from_parsed_template)

        print(f"Le fichier HTML a été généré avec succès à : {output_file_path}")